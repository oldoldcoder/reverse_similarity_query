#include "stdio.h"
#include "utils.h"
#include "string.h"
#include "RSQ_data_structure.h"
#include "iostream"
int K_MAX = -1;
/*-------------------------方法实现---------------------------*/
// 从文件读取数据填充两个set
RESULT RSQ_read_data(RSQ_data * data,char * dataFilePath){
    FILE* file;
    if (dataFilePath != NULL) {
        file = fopen(dataFilePath, "r");
    }
    else {
        file = fopen(RSQ_DATA_FILE, "r");
    }
   
    if(file == NULL){
        perror("Error opening file");
        return ERROR;
    }
    char *line = NULL;
    size_t len = 0;
    // 初始化data
    int xlen , ylen, dim;
    // 读取三个数字
    fscanf(file, "%d %d %d %d\n", &xlen,&ylen, &dim,&K_MAX); // 读取第一行中的两个数字
    data->xn = xlen;
    data->yn = ylen;
    data->dim = dim;

    data->en_x = (set_x **) malloc(xlen* sizeof (set_x *));
    data->open_y = (set_y **) malloc(ylen * sizeof (set_y *));
    if(data->en_x == NULL || data->open_y  == NULL){
        fprintf(stderr,"Memory allocation failed.\n");
        return ERROR;
    }
    // 首先读取x的内容
    for(int i = 0 ; i < xlen ; i++){
        data->en_x[i] = (set_x *) malloc(sizeof (set_x));
        data->en_x[i]->dim = dim;
        // 分配内存空间
        data->en_x[i]->de_data = (BIGNUM **) malloc(dim * sizeof (BIGNUM *));
        data->en_x[i]->en_data = (eTPSS **) malloc(dim * sizeof (eTPSS *));
        if(data->en_x[i] == NULL || data->en_x[i]->de_data == NULL || data->en_x[i]->en_data == NULL){
            fprintf(stderr,"Memory allocation failed.\n");
            return ERROR;
        }
    }
    for(int i = 0 ; i < ylen ; i++){
        data->open_y[i] = (set_y *) malloc(sizeof (set_y));
        data->open_y[i]->dim = dim;
        // 分配内存空间
        data->open_y[i]->single_data = (BIGNUM **) malloc(dim * sizeof (BIGNUM *));

        if(data->open_y[i] == NULL || data->open_y[i]->single_data == NULL){
            fprintf(stderr,"Memory allocation failed.\n");
            return ERROR;
        }
    }
    // 先读取x的内容，然后读取y的内容
    int n = 0;
    while (getline(&line, &len, file) != -1) {
        int idx = 0;
        char *token = strtok(line, ",");
        while (token != NULL) {
            BIGNUM *tmp = BN_CTX_get(CTX);
            fflush(stdout);
            // 转换为BIGNUM函数
            if(!BN_dec2bn(&tmp,token)){
                fprintf(stdout,"file have some error format about data\n");
                return ERROR;
            }
            if(n < xlen){
                data->en_x[n]->de_data[idx++] = tmp;
            }else{
                data->open_y[n - xlen]->single_data[idx++] = tmp;
            }
            token = strtok(NULL, ",");
        }
        if(idx != dim){
            fprintf(stderr,"Error reading data dimension from file\n");
            return ERROR;
        }
        n++;
    }
    if(n != (xlen + ylen)){
        fprintf(stderr,"Error reading data rows from file\n");
        return ERROR;
    }

    if(data->xn >= 10000){
        data->is_mul_thread_flag = TRUE;
        int chunk = data->xn / THREAD_NUM;
        // 开辟空间准备存储
        data->batch = new vector<vector<set_x *>*>;
        for(int i = 0 ; i < THREAD_NUM ; i ++){
            int limit = (i + 1) * chunk;
            if(i + 1 == THREAD_NUM){
                limit = data->xn;
            }
            auto * vec = new vector<set_x *>;
            for(int j = i * chunk ; j < limit ; ++j){
                vec->push_back(data->en_x[j]);
            }
//            cout<< "i : " << i << endl;
//            for(auto it : *vec){
//                for(int j = 0; j < dim; ++j){
//                    char * str = BN_bn2dec(it->de_data[j]);
//                    cout<< str << " ";
//                }
//                cout << endl;
//            }
            // 存入
            data->batch->push_back(vec);
        }
    }else{
        data->batch = NULL;
        data->is_mul_thread_flag = FALSE;
    }


    fclose(file);
    return SUCCESS;
}
// 对于x数据的解密
RESULT RSQ_decrypt_setx(RSQ_data * data){
    if(data->is_mul_thread_flag == TRUE){
        cerr << "mul_thread_decrypt is not write" <<endl;
    }else{
        int xn = data->xn;
        int dim = data->en_x[0]->dim;
        for(int i = 0 ; i < xn ; ++i){
            for(int j = 0; j < dim ; ++j){
                // 初始化etpss
                eTPSS * encrypt = data->en_x[i]->en_data[j];
                if(encrypt == NULL){
                    fprintf(stderr,"%s[%d]错误的数据\n",__func__ ,__LINE__);
                    return ERROR;
                }
                if(et_Recover(data->en_x[i]->de_data[j],encrypt) != ETPSS_SUCCESS){
                    fprintf(stderr,"et_recover failed!\n");
                    return ERROR;
                }
            }
        }
        return SUCCESS;
    }
}
// 对于x数据的加密
RESULT RSQ_encrypt_setx(RSQ_data * data){


    /*for(int i = 0 ; i < THREAD_NUM ; ++i){
        cout<< "i : " << i << endl;
        for(int j = 0 ; j < (*data->batch)[0]->size() ; ++j)
            for(int z = 0 ; z < 4 ; ++z){
                char * str = BN_bn2dec((*(*data->batch)[i])[j]->de_data[z]);
                cout<< str << " ";

            }
        cout << endl;
    }*/

    // 从de_data加密到en_data去,逐行加密，逐维度加密
    int dim = data->dim;
    // 如果是多线程模式则对于另外一个数据进行加密
    if(data->is_mul_thread_flag == TRUE){

        for(int i = 0 ; i < THREAD_NUM ; ++i){
            for(int j = 0 ; j < (*data->batch)[0]->size() ; ++j){
                for(int z = 0 ; z < dim ; ++z){
                    eTPSS * encrypt = (eTPSS * ) malloc(sizeof (eTPSS));
                    if(encrypt == NULL){
                        fprintf(stderr,"Memory allocation failed.\n");
                        return ERROR;
                    }
                    init_eTPSS(encrypt);
                    if(et_Share(encrypt,(*(*data->batch)[i])[j]->de_data[z]) != ETPSS_SUCCESS){
                        fprintf(stderr,"et_Share failed!\n");
                        return ERROR;
                    }
                    // 填充过去
                     (*(*data->batch)[i])[j]->en_data[z] = encrypt;
                }
            }
        }
    }else{
        int xn = data->xn;
        for(int i = 0 ; i < xn ; ++i){
            for(int j = 0; j < dim ; ++j){
                // 初始化etpss
                eTPSS * encrypt = (eTPSS * ) malloc(sizeof (eTPSS));
                if(encrypt == NULL){
                    fprintf(stderr,"Memory allocation failed.\n");
                    return ERROR;
                }
                init_eTPSS(encrypt);
                if(et_Share(encrypt,data->en_x[i]->de_data[j]) != ETPSS_SUCCESS){
                    fprintf(stderr,"et_Share failed!\n");
                    return ERROR;
                }
                data->en_x[i]->en_data[j] = encrypt;
            }
        }
    }


    return SUCCESS;
}
// 释放做占用的空间数据
RESULT RSQ_free(RSQ_data * data){
    int xn = data->xn;
    int yn = data->yn;
    int dim = data->dim;

    // 删除set x的部分
    for(int i = 0 ; i < xn ; ++i){
        for(int j = 0 ; j < dim ; ++j){
            BN_clear(data->en_x[i]->de_data[j]);
            free_eTPSS(data->en_x[i]->en_data[j]);
            free(data->en_x[i]->en_data[j]);

        }
        free(data->en_x[i]);
    }
    free(data->en_x);
    // 删除set y的部分
    for(int i = 0 ; i < yn ; ++i){
        for(int j = 0 ; j < dim ; ++j) {
            BN_clear(data->open_y[i]->single_data[j]);
        }
        free(data->open_y[i]->single_data);
    }

    free(data->open_y);
    return SUCCESS;
}