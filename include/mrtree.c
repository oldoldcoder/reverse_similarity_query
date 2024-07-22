#include <string.h>
#include "stdio.h"
#include "mrtree.h"
#include "utils.h"
#include "RSQ_data_structure.h"


/**
这释放内存我有三不释，第一不，我自己申请的我不释放，因为我善，不忍心看着为自己曾经工作过的空间没了，
 第二不，不会再用的我不释，因为它忠，我总不能卸磨杀驴，
 第三不，自定义释放方法的我不释放，因为我追求质朴，只用最原始的free
 * */

/**--------------------JNA调用函数-------------------*/
EXPORT_SYMBOL RESULT init_algo(char* dataFilePath, RSQ_data* data, mr_tree* tree){

    initialize_Constant();
    if( RSQ_read_data(data, dataFilePath) != SUCCESS) {
        return ERROR;
    }
 
    // 对数据进行加密
    if( RSQ_encrypt_setx(data) != SUCCESS) {
        return ERROR;
    }
    /*-------------------创建树阶段--------------------*/
    Heap * heap;
    eTPSS  *** dis = (eTPSS ***) malloc(data->xn * sizeof (eTPSS **));
    // 初始化我们的dis的内存空间
    if(mrtree_init_distance(dis,&heap,data->xn,data->yn) != SUCCESS) {
        return ERROR;
    }
    if(mrtree_compute_xy_distance(heap,data,dis) != SUCCESS) {
        return ERROR;
    }
   
    // 初始化最初的节点
    mr_node** nodes = (mr_node**)malloc(data->xn * sizeof(mr_node*));
    if(mrtree_init_origin_node(dis, data,nodes) != SUCCESS) {
        return ERROR;
    }

    // 构建树
    tree->root = mrtree_create_tree(nodes,data->xn);
    if (tree->root == NULL) {
        return ERROR;
    }
    heap_free(heap,data->yn);
    // 释放dis指针，具体dis指针指向的内存空间需要释放树的时候才能释放这些内存
    free(dis);
    return SUCCESS;
}
EXPORT_SYMBOL RESULT query_algo(RSQ_data* data, mr_tree* tree, char* queryFilePath, char* resultFilePath) {
    search_req req;
    search_resp  resp;
    // 初始化查询
    if (mrtree_init_query_param(&req, &resp, data->dim, queryFilePath) != SUCCESS) {
        return ERROR;
    }
    if(mrtree_search(tree, &req, &resp) != SUCCESS) {
        return ERROR;
    }
    if(mrtree_write_resp(&req, &resp, data->dim,resultFilePath) != SUCCESS) {
        return ERROR;
    }
    // TODO 释放req和resp的内存内容
    mrtree_free_search_param(&req, &resp,data->dim);

    return SUCCESS;
}
EXPORT_SYMBOL RESULT free_algo( RSQ_data* data, mr_tree* tree){

    K_MAX = -1;
    RSQ_free(data);
    //  TODO 关于树的释放逻辑
    mrtree_free(tree->root);
    return SUCCESS;
}
/*-------------------------方法实现---------------------------*/
// 初始化distance
RESULT mrtree_init_distance(eTPSS ***dis,Heap ** heap,int x_len,int y_len){

    // 如果y点的数量小于k_max报错
    if(y_len < K_MAX){
        fprintf(stderr,"mrtree_init_distance:The length of y is less than kmax, and initialization work cannot be completed\n");
        return ERROR;
    }

    *heap = createHeap(y_len,FALSE);
    for(int i = 0 ; i < x_len ; ++i){
        // 初始化堆结构
        dis[i] = (eTPSS **) malloc(K_MAX * sizeof (eTPSS *));
    }
    return SUCCESS;
}
/*// 计算距离,完毕之后会使用sort方法对于数据进行排序
RESULT mrtree_compute_xy_distance(Heap * h,RSQ_data * data,eTPSS *** dis){
    // 计算距离，注意是密文计算
    int x_len = data->xn;
    int y_len = data->yn;
    int dim = data->dim;

    eTPSS tmp,tmp2;
    init_eTPSS(&tmp);
    init_eTPSS(&tmp2);
    eTPSS * ousDis = (eTPSS * ) malloc(sizeof (eTPSS));
    init_eTPSS(ousDis);
    // 计算距离插入distance
    for(int i = 0 ; i < x_len ; ++i){
        // 获取了不同的维度
        eTPSS  ** dimx =  data->en_x[i]->en_data;
        for(int j = 0 ; j < y_len ; ++j){
            // 计算一个欧式距离的结果
            et_Share(ousDis,ZERO);
            BIGNUM  ** dimy = data->open_y[j]->single_data;
            // 进行计算欧式距离的值
            for(int z = 0 ; z < dim; ++z){
                et_Sub_cal_res(&tmp,dimx[z],dimy[z]);
                // 计算距离的平方
                et_Mul(&tmp2,&tmp,&tmp);
                // 加和
                et_Add(ousDis,ousDis,&tmp2);

            }
            insert(h,ousDis);
        }
        clock_t start_time;
        if(i == 0){
            start_time = clock();
        }
        // 弹出前k_max个数值，然后我们的情况我们的heap重新填充值
        heap_PopK_max_Val(h,K_MAX,dis[i]);
        if(i == 100 || i == 10){
            clock_t end_time = clock();
            double execution_time = ((double) (end_time - start_time)) / CLOCKS_PER_SEC * 1000;
            printf("%s次计算距离的时间：%f 毫秒\n", (i == 100 ? "100次":"10次"),execution_time);
            fflush(stdout);
        }
        heapClear(h);
        // debug打印内容
        *//*for(int j = 0 ; j < dim ; ++j){
            printDebugInfo(NULL,dimx[j],__func__ ,__LINE__,"数据为");
        }
        for(int j = 0 ; j < K_MAX ; ++j){
            printDebugInfo(NULL,dis[i][j],__func__ ,__LINE__,"前KMAX距离的数据为");
        }*//*
    }

    heap_free(h,y_len);
    free_eTPSS(ousDis);
    free(ousDis);
    return SUCCESS;
}
 */ // 这里是使用密文加密计算距离的地方
// 计算距离,完毕之后会使用sort方法对于数据进行排序
RESULT mrtree_compute_xy_distance(Heap * h,RSQ_data * data,eTPSS *** dis){
    // 计算距离，注意是密文计算
    int x_len = data->xn;
    int y_len = data->yn;
    int dim = data->dim;

    BIGNUM * tmp = BN_CTX_get(CTX);
    BIGNUM * tmp2 = BN_CTX_get(CTX);
    BIGNUM * ousDis = BN_CTX_get(CTX);
    for(int i = 0 ; i < x_len ; ++i){
        printf("i:%d\n",i);
        for(int j = 0 ; j < y_len ; ++j){

            // 重新给0值
            BN_set_word(ousDis,0);
            for(int z = 0 ; z < dim; ++z) {
                BN_sub(tmp,data->en_x[i]->de_data[z],data->open_y[j]->single_data[z]);
                BN_mul(tmp2,tmp,tmp,CTX);
                BN_add(ousDis,ousDis,tmp2);
            }
            // 插入这个数据点
            insert(h,ousDis);
        }
        // 弹出前k_max个数值，然后我们的情况我们的heap重新填充值
        heap_PopK_max_Val(h,K_MAX,dis[i]);
        heapClear(h);
    }


    BN_clear(tmp);
    BN_clear(tmp2);
    BN_clear(ousDis);
    return SUCCESS;
}
// 计算不同的x之间的距离
RESULT mrtree_compute_inner_distance(int * map,mr_node ** nodes,int size){
    // 设定一个数组防止重复选取
    int dim = nodes[0]->dim;
    int n = size;
    int * choose = (int *) malloc(sizeof (int) * n);
    int res = -1;

    int idxJ = -1;
    // 初始化
    for(int i = 0 ; i < n ; ++i)
        choose[i] = 0;
    // 用作叶子节点的时候
    eTPSS ** targetA = NULL;
    eTPSS ** targetB = NULL;

    // 申请内存块用来存放非叶子节点的数据
    eTPSS  ** targetC = (eTPSS **) malloc(sizeof (eTPSS *) * dim);
    eTPSS  ** targetD = (eTPSS **) malloc(sizeof (eTPSS *) * dim);
    for(int i = 0 ; i < dim ; ++i){
        targetC[i] = (eTPSS *) malloc(sizeof (eTPSS));
        targetD[i] = (eTPSS *) malloc(sizeof (eTPSS));
        init_eTPSS(targetC[i]);
        init_eTPSS(targetD[i]);
        // 初始化0值
        //et_Share(targetC[i],ZERO)
    }
    eTPSS tmp,tmp2;
    init_eTPSS(&tmp);
    init_eTPSS(&tmp2);
    eTPSS sum;
    init_eTPSS(&sum);
    eTPSS min;
    init_eTPSS(&min);
    BIGNUM * init_min = BN_CTX_get(CTX);
    BN_set_word(init_min,999999999999L);

    BIGNUM * two = BN_CTX_get(CTX);
    BN_set_word(two,2);
    for(int i = 0 ; i < size; ++i) {
        printf("i:%d\n",i);
        if(choose[i] == 1)
            continue;
        et_Share(&min,init_min);
        if (nodes[i]->is_left_ndoe == TRUE) {
            targetA = nodes[i]->data->en_data;
            for (int j = 0; j < size; ++j) {
                if (j == i)
                    continue;
                // 已经被选了
                if (choose[j] == 1)
                    continue;
                if (nodes[j]->is_left_ndoe == TRUE){
                    targetB = nodes[j]->data->en_data;
                    et_Share(&tmp, ZERO);
                    et_Share(&tmp2,ZERO);
                    et_Share(&sum, ZERO);
                    // 计算两个etpss之间的距离
                    for (int z = 0; z < dim; ++z) {

                        et_Sub_cal_res_o(&tmp, targetA[z], targetB[z]);// 计算距离的平方

                        et_ScalP(&tmp,&tmp,two);
                        et_Mul(&tmp2,&tmp,&tmp);
                        et_Add(&sum, &sum, &tmp2);
                    }
                }else{
                    // 计算叶子节点和非叶子节点
                    for(int z = 0 ; z < dim ; ++ z){
                        // 当前维度的最小值
                        eTPSS * dim_min = nodes[z]->range[j][0];
                        eTPSS * dim_max = nodes[z]->range[j][1];
                        // 计算中值点
                        et_Add(&tmp,dim_min,dim_max);
                        // copy过去数值
                        et_Copy(targetD[j],&tmp);
                    }
                    et_Share(&tmp, ZERO);
                    et_Share(&tmp2,ZERO);
                    et_Share(&sum, ZERO);
                    // 计算两个etpss之间的距离
                    for (int z = 0; z < dim; ++z) {
                        et_ScalP(&tmp2,targetA[z],two);
                        et_Sub_cal_res_o(&tmp, &tmp2, targetD[z]);// 计算距离的平方
                        et_Mul(&tmp2,&tmp,&tmp);
                        et_Add(&sum, &sum, &tmp2);
                    }
                }
                // 寻找 最小的距离
                et_Sub(&res, &min, &sum);
                if (res == 0){
                    et_Copy(&min,&sum);
                    idxJ = j;
                }
            }
        }else{
            et_Share(&tmp, ZERO);
            et_Share(&tmp2,ZERO);
            // 非叶子节点我们需要将中点找出来，使用最大值和最小值的中间值
            for(int j = 0 ; j < dim ; ++ j){
                // 当前维度的最小值
                eTPSS * dim_min = nodes[i]->range[j][0];
                eTPSS * dim_max = nodes[i]->range[j][1];
                // 计算中值点
                et_Add(&tmp,dim_min,dim_max);
                // copy过去数值
                et_Copy(targetC[j],&tmp);
            }

            for (int j = 0; j < size; ++j) {
                if (j == i)
                    continue;
                // 已经被选了
                if (choose[j] == 1 || choose[i] == 1)
                    continue;
                if (nodes[j]->is_left_ndoe == TRUE){
                    targetB = nodes[j]->data->en_data;
                    et_Share(&tmp, ZERO);
                    et_Share(&sum, ZERO);
                    et_Share(&tmp2,ZERO);
                    // 计算两个etpss之间的距离
                    for (int z = 0; z < dim; ++z) {
                        et_ScalP(&tmp2,targetB[z],two);
                        et_Sub_cal_res_o(&tmp, targetC[z], &tmp2);// 计算距离的平方
                        et_Mul(&tmp2,&tmp,&tmp);
                        et_Add(&sum, &sum, &tmp2);
                    }
                }else{
                    // 计算非叶子节点和非叶子节点
                    for(int z = 0 ; z < dim ; ++ z){
                        // 当前维度的最小值
                        eTPSS * dim_min = nodes[j]->range[z][0];
                        eTPSS * dim_max = nodes[j]->range[z][1];
                        // 计算中值点
                        et_Add(&tmp,dim_min,dim_max);
                        // copy过去数值
                        et_Copy(targetD[z],&tmp);
                    }
                    et_Share(&tmp, ZERO);
                    et_Share(&tmp2,ZERO);
                    et_Share(&sum, ZERO);
                    // 计算两个etpss之间的距离
                    for (int z = 0; z < dim; ++z) {
                        et_Sub_cal_res_o(&tmp, targetC[z], targetD[z]);// 计算距离的平方
                        et_Mul(&tmp2,&tmp,&tmp);
                        et_Add(&sum, &sum, &tmp2);
                    }
                }

                // 寻找 最小的距离
                et_Sub(&res, &min, &sum);
                if (res == 0){
                    et_Copy(&min,&sum);
                    idxJ = j;
                }
            }

        }

        if(idxJ != -1){
            // 记录最相近的点
            choose[i] = 1;
            choose[idxJ] = 1;
            map[i] = idxJ;
            map[idxJ] = i;
            // idxJ恢复原值
            idxJ = -1;
        }
    }
    BN_clear(init_min);
    free_eTPSS(&tmp);
    free_eTPSS(&sum);
    free_eTPSS(&tmp2);
    free_eTPSS(&min);
    // 释放内存
    for(int i = 0 ; i < dim ; ++i){
        free_eTPSS(targetC[i]);
        free_eTPSS(targetD[i]);
        free(targetC[i]);
        free(targetD[i]);
    }
    free(targetC);
    free(targetD);

    BN_clear(two);
    return SUCCESS;

}
// TODO 内存空间的释放
// 创建MR树的过程，返回的是根节点，自底向上构建树
mr_node * mrtree_create_tree(mr_node ** nodes,int size){
    //printShowNodeVal(nodes,size);
    if(size == 1)
        return nodes[0];
    int res = -1;

    // 首先计算如何合并
    int * map = (int *) malloc(sizeof(int) *size);
    for(int i = 0 ; i < size ; ++i){
        map[i] = -1;
    }
    if(mrtree_compute_inner_distance(map,nodes,size) != SUCCESS){
        fprintf(stderr,"%s[%d]:计算节点内部距离出错\n",__func__ ,__LINE__);
        return NULL;
    }
    // 创建下一个数组来存储我们的点
    int next_size = 0;
    if(size % 2 != 0){
        next_size = size / 2 + 1;
    }else{
        next_size = size / 2;
    }
    mr_node ** arr  = (mr_node **) malloc(next_size * sizeof(mr_node *));
    int next_idx = 0;
    // 按照计算得到的，我们进行合并，合并树,假如节点不是偶数，那么我们会遗留一个点到上一次
    // 遍历整个map，将-1的遗留下一层，将有对应的创建，同时将还没遍历到的设置为负二
    for(int i = 0 ; i < size ; ++i){
        if(map[i] == -1){
            // 遗留下一层
            arr[next_idx++] = nodes[i];
            break;
        }
        if(map[i] == -2)
            continue;
        // 如果map有对应
        int idxJ = map[i];
        map[idxJ] = -2;
        mr_node * targetA = nodes[i];
        mr_node * targetB = nodes[idxJ];
        // 合并两个mr_node，每个维度选择最小值，每个维度选择最大值
        mr_node * newNode = (mr_node *) malloc(sizeof (mr_node));
        mrtree_init_node(newNode,targetA->dim,FALSE,targetA,targetB,NULL,NULL,NULL);
        for(int j = 0 ; j < newNode->dim ; ++ j){
            eTPSS  * min = (eTPSS *) malloc(sizeof (eTPSS));
            init_eTPSS(min);
            eTPSS  * max = (eTPSS *) malloc(sizeof (eTPSS));
            init_eTPSS(max);
            et_Sub(&res,targetA->range[j][0],targetB->range[j][0]);

            if(res == 0){
                et_Copy(min,targetB->range[j][0]);
            }else{
                et_Copy(min,targetA->range[j][0]);
            }
            et_Sub(&res,targetA->range[j][1],targetB->range[j][1]);
            if(res == 0){
                et_Copy(max,targetA->range[j][1]);
            }else{
                et_Copy(max,targetB->range[j][1]);
            }
            newNode->range[j][0] = min;
            newNode->range[j][1] = max;
        }
        arr[next_idx ++] = newNode;
    }
    free(map);
    // 释放存储指针的地址，而不释放具体的值
    free(nodes);
    return mrtree_create_tree(arr,next_size);
}
void mrtree_search_o(mr_node* root,search_req * req, search_resp * resp){
    // 判断是否在范围内
    if(root == NULL)
        return;
    int res = -1;
    int dim = root->dim;
    if(root->is_left_ndoe == TRUE){
        eTPSS tmp,tmp2;
        init_eTPSS(&tmp);
        init_eTPSS(&tmp2);
        eTPSS sum;
        init_eTPSS(&sum);
        et_Share(&sum,ZERO);
        for(int i = 0; i < dim ; ++i){
            // 判断是否在范围内，假如到resp中进行返回
            et_Sub_cal_res_o(&tmp,root->data->en_data[i],req->y[i]);
            et_Mul(&tmp2,&tmp,&tmp);
            et_Add(&sum, &sum, &tmp2);
        }
        // 判断和第k个距离的大小
        eTPSS * kth_distance = root->distance[req->k - 1];
        et_Sub(&res,kth_distance,&sum);
        if(res != 1){
            // 插入队列
            res_node * node = (res_node *) malloc(sizeof (res_node));
            node->data = root->data;
            node->next = NULL;
            if (resp->root == NULL){
                resp->root = node;
                resp->now = node;
            }else{
                resp->now->next = node;
                resp->now = node;
            }
        }
        return;
    }

    int is_in_range = TRUE;
    eTPSS  *** range = root->range;
    for(int i = 0 ; i < dim ; ++i){
        eTPSS * min = range[i][0];
        eTPSS * max = range[i][1];
        // 判断是否在范围内
        et_Sub(&res,req->y[i],min);
        if(res == 1){
            is_in_range = FALSE;
            break;
        }
        et_Sub(&res,req->y[i],max);
        if(res == 0){
            is_in_range = FALSE;
            break;
        }
    }
    if(is_in_range == TRUE){
        // 继续查找子树,否则退出
        mrtree_search_o(root->left,req,resp);
        mrtree_search_o(root->right,req,resp);
    }
    return;
}
// 进行反向查询
RESULT mrtree_search(mr_tree * tree,search_req * req, search_resp * resp){
    if(tree->root == NULL)
        return ERROR;
    // 可以不用查了，k大于了我们设定的kmax值
    if(req->k > K_MAX)
        return ERROR;
    mrtree_search_o(tree->root,req,resp);
    return SUCCESS;
}


// 初始化查询以及我们的结果
RESULT mrtree_init_query_param(search_req * req, search_resp * resp,int dim, char* queryFilePath){
    FILE* file;
    if (queryFilePath != NULL) {
        file = fopen(queryFilePath, "r");
    }else {
        file = fopen(REQ_DATA_PATH, "r");
    }
    if(file == NULL){
        perror("Error opening file");
        return ERROR;
    }
    // 从文件读取内容
    req->k = K_MAX;
    req->y = (eTPSS **) malloc(dim * sizeof (eTPSS*));
    for(int i = 0 ; i < dim ; ++i){
        eTPSS  * d  = (eTPSS *) malloc(sizeof (eTPSS));
        init_eTPSS(d);
        req->y[i] = d;
    }

    char *line = NULL;
    size_t len = 0;
    getline(&line, &len, file);
    int idx = 0;
    char *token = strtok(line, " ");
    while (token != NULL) {
        BIGNUM *tmp = BN_CTX_get(CTX);
        fflush(stdout);
        // 转换为BIGNUM函数
        if(!BN_dec2bn(&tmp,token)){
            fprintf(stdout,"file have some error format about data\n");
            return ERROR;
        }
        et_Share(req->y[idx++],tmp);
        BN_clear(tmp);
        token = strtok(NULL, " ");
    }
    if(idx != dim){
        fprintf(stderr,"Error reading data dimension from file\n");
        return ERROR;
    }

    resp->root = resp->now = NULL;
    return SUCCESS;
}

// 查询结果的写入
RESULT mrtree_write_resp(search_req * req, search_resp * resp,int dim, char* resultPath){
    // 读取resp文件的内容然后写入进去
    FILE* file = fopen(resultPath, "w");
    if(file == NULL){
        perror("Error opening file");
        return ERROR;
    }
    BIGNUM * tmp = BN_CTX_get(CTX);
    char *str ;
    fprintf(file,"The search rep\ndim:%d\nreq data: ",dim);
    for(int i = 0 ; i < dim ; ++i){
        et_Recover(tmp,req->y[i]);
        str = BN_bn2dec(tmp);
        fprintf(file,"%s ",str);
    }
    fprintf(file,"\nsearch res:\n");
    res_node *root = resp->root;
    int idx = 0;
    while(root != NULL){
        fprintf(file,"%d. ",idx);
        for(int i = 0 ; i < dim ; ++i){
            et_Recover(tmp,root->data->en_data[i]);
            str = BN_bn2dec(tmp);
            fprintf(file,"%s ",str);
        }
        idx++;
        fprintf(file,"\n");
        root = root->next;
    }
    // 关闭文件
    fclose(file);
    return SUCCESS;
}
RESULT mrtree_init_node(mr_node * node,int dim,int is_left, mr_node * right, mr_node * left,set_x * data,eTPSS ** distance, eTPSS * maxDistance){
    node->is_left_ndoe = is_left;
    node->right = right;
    node->left = left;
    node->data = data;
    node->dim = dim;
    // 开辟range的空间以及其他的空间
    node->range = (eTPSS ***) malloc( dim * sizeof (eTPSS **));
    for(int i = 0 ; i < dim ; ++i){
        node->range[i] = (eTPSS **) malloc( 2 * sizeof (eTPSS *));
    }
    node->maxDistance = maxDistance;
    node->distance = distance;
    return SUCCESS;
}

// 释放内存的点
RESULT mrtree_free(mr_node* root){
    int dim = root->dim;
    if (root->is_left_ndoe == TRUE) {
        for (int i = 0; i < dim; ++i) {
            free_eTPSS(root->range[i][0]);
            free_eTPSS(root->range[i][1]);
            free(root->range[i]);
        }
        free(root->range);
        free(root->distance);
        free_eTPSS(root->maxDistance);
        free(root->maxDistance);
    }
    else {
        for (int i = 0; i < dim; ++i) {
           
            free_eTPSS(root->range[i][0]);
            free_eTPSS(root->range[i][1]);
            free(root->range[i]);
        }
        free(root->range);
    }
    
    if (root->left != NULL) {
        mrtree_free(root->left);
    }
    else if(root->right != NULL) {
        mrtree_free(root->right);
    }
    free(root);
    return SUCCESS;
}
// 释放resp的节点
RESULT mrtree_free_search(search_req * req,search_resp * resp,int dim){

    for(int i = 0 ; i < dim ; ++i){
        free_eTPSS(req->y[i]);
        free(req->y[i]);
    }
    free(req->y);
    res_node * root = resp->root;
    while(root != NULL){
        res_node *next = root->next;
        free(root);
        // ps:root里面的set_x不能free掉
        root = next;
    }
    return SUCCESS;
}

// 初始化最初的节点
RESULT mrtree_init_origin_node(eTPSS *** dis,RSQ_data * total,mr_node ** nodes){
    int xl = total->xn;

    int idx = 0;
    if(nodes == NULL)
        return ERROR;
    for(int i = 0 ; i < xl ; ++i){
        mr_node * node =(mr_node *) malloc(sizeof (mr_node));
        if(node == NULL)
            return ERROR;
        // 将heap转换为etpss **
        mrtree_init_node(node,total->dim,TRUE,NULL,NULL,total->en_x[i],dis[i],dis[i][K_MAX - 1]);
        // 为range进行赋值
        for(int j = 0 ; j < total->dim ; ++j){
            // 最小值
            node->range[j][0] = (eTPSS *) malloc(sizeof (eTPSS));
            init_eTPSS(node->range[j][0]);
            et_Sub_cal_res_o(node->range[j][0],total->en_x[i]->en_data[j],dis[i][K_MAX - 1]);

            // 最大值
            node->range[j][1] = (eTPSS *) malloc(sizeof (eTPSS));
            init_eTPSS(node->range[j][1]);
            et_Add(node->range[j][1],total->en_x[i]->en_data[j],dis[i][K_MAX - 1]);

        }

        nodes[idx ++] = node;
    }
    return SUCCESS;
}

RESULT mrtree_free_search_param(search_req* req, search_resp* resp,int dim) {
    for (int i = 0; i < dim; ++i) {
        free_eTPSS(req->y[i]);
        free(req->y[i]);
    }
    free(req->y);

    // 释放resp的参数
    res_node* node = resp->root;
    while (node != NULL) {
        res_node* tmp = node->next;
        free(node);
        node = tmp;
    }

    return SUCCESS;
}

void printShowNodeVal(mr_node **nodes,int size){
    BIGNUM * tt = BN_CTX_get(CTX);
    for(int i = 0 ; i < size ; ++i){
        printf("%d个节点:",i);
        if(nodes[i]->is_left_ndoe == TRUE){
            printf("这是一个叶子节点--");
            for(int j = 0 ; j < nodes[0]->dim ; ++j){
                et_Recover(tt,nodes[i]->data->en_data[j]);
                char * str = BN_bn2dec(tt);
                printf("%s ",str);
            }
        }else{
            printf("这是一个合成的范围节点--");
            for(int j = 0 ; j < nodes[0]->dim ; ++j){
                et_Recover(tt,nodes[i]->range[j][0]);
                char * str = BN_bn2dec(tt);
                printf("%d维度最小值%s ",j,str);
                et_Recover(tt,nodes[i]->range[j][1]);
                str = BN_bn2dec(tt);
                printf("%d维度最大值%s ",j,str);
            }
        }
        printf("\n");
    }
    BN_clear(tt);
    fflush(stdout);
}