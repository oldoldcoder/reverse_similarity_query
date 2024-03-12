#include "stdio.h"
#include "mrtree.h"
#include "utils.h"
#include "RSQ_data_structure.h"


/**
这释放内存我有三不释，第一不，我自己申请的我不释放，因为我善，不忍心看着为自己曾经工作过的空间没了，
 第二不，不会再用的我不释，因为它忠，我总不能卸磨杀驴，
 第三不，自定义释放方法的我不释放，因为我追求质朴，只用最原始的free
 * */
/*-------------------------方法实现---------------------------*/
// 初始化distance
RESULT mrtree_init_distance(distance * dis,int x_len,int y_len){
    dis->y_len = y_len;
    dis->x_len = x_len;

    // 如果y点的数量小于k_max报错
    if(y_len < K_MAX){
        fprintf(stderr,"mrtree_init_distance:The length of y is less than kmax, and initialization work cannot be completed\n");
        return ERROR;
    }
    // 初始化distance
    dis->d = (Heap **) malloc(sizeof (Heap *) * x_len);
    for(int i = 0 ; i < x_len ; ++i){
        // 初始化堆结构
        dis->d[i] = createHeap(y_len);
    }
    return SUCCESS;
}
// 计算距离,完毕之后会使用sort方法对于数据进行排序
RESULT mrtree_compute_xy_distance(distance * dis,RSQ_data * data){
    // 计算距离，注意是密文计算
    int x_len = data->xn;
    int y_len = data->yn;
    int dim = data->dim;

    eTPSS tmp,tmp2;
    init_eTPSS(&tmp);
    init_eTPSS(&tmp2);
    // 计算距离插入distance
    for(int i = 0 ; i < x_len ; ++i){
        // 获取了不同的维度
        eTPSS  ** dimx =  data->en_x[i]->en_data;
        for(int j = 0 ; j < y_len ; ++j){
            // 计算一个欧式距离的结果
            eTPSS * ousDis = (eTPSS * ) malloc(sizeof (eTPSS));
            init_eTPSS(ousDis);
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
            // 插入堆里面
            insert(dis->d[i],ousDis);
        }
    }
    // 对于每个heap我们进行排序
    for(int i = 0 ; i < x_len ; ++i){
        heapSort(dis->d[i]);
    }
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
    BN_set_word(init_min,2);
    for(int i = 0 ; i < size; ++i) {
        et_Share(&min,init_min);
        if (nodes[i]->is_left_ndoe == TRUE) {
            targetA = nodes[i]->data->en_data;
            for (int j = 0; j < size; ++j) {
                if (j == i)
                    continue;
                // 已经被选了
                if (choose[j] == 1 || choose[i] == 1)
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
                        et_ScalP(targetA[z],targetA[z],two);
                        et_Sub_cal_res_o(&tmp, targetA[z], targetD[z]);// 计算距离的平方
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
                eTPSS * dim_min = nodes[j]->range[j][0];
                eTPSS * dim_max = nodes[j]->range[j][1];
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
                        et_ScalP(targetB[z],targetB[z],two);
                        et_Sub_cal_res_o(&tmp, targetC[z], targetB[z]);// 计算距离的平方
                        et_Mul(&tmp2,&tmp,&tmp);
                        et_Add(&sum, &sum, &tmp2);
                    }
                }else{
                    // 计算非叶子节点和非叶子节点
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

        // 记录最相近的点
        choose[i] = 1;
        choose[idxJ] = 1;
        map[i] = idxJ;
        map[idxJ] = i;
        // idxJ恢复原值
        idxJ = -1;
    }
    BN_clear(init_min);
    free_eTPSS(&tmp);
    free_eTPSS(&sum);
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
// 创建MR树的过程，返回的是根节点，自底向上构建树
mr_node * mrtree_create_tree(mr_node ** nodes,int size){
    if(size == 1)
        return nodes[0];
    int res = -1;

    // 首先计算如何合并
    int * map = (int *) malloc(sizeof(int) *size);
    for(int i = 0 ; i < size ; ++i){
        map[i] = 0;
    }
    mrtree_compute_inner_distance(map,nodes,size);
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
            arr[next_size++] = nodes[i];
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
                et_Copy(min,targetB->range[j][1]);
            }
            newNode->range[j][0] = min;
            newNode->range[j][1] = max;
        }
        arr[next_idx ++] = newNode;
    }
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
RESULT mrtree_init_query_param(search_req * req, search_resp * resp,int k,set_y * y ){
    req->k = k;
    req->y = (eTPSS **) malloc(y->dim * sizeof (eTPSS*));
    for(int i = 0 ; i < y->dim ; ++i){
        eTPSS  * d  = (eTPSS *) malloc(sizeof (eTPSS));
        init_eTPSS(d);
        et_Share(d,y->single_data[i]);
        req->y[i] = d;
    }
    resp->root = resp->now = NULL;
    return SUCCESS;
}

// 查询结果的写入
RESULT mrtree_write_resp(search_req * req, search_resp * resp){
    // 读取resp文件的内容然后写入进去
    FILE  * file = fopen(RESP_DATA_PATH,"w");
    if(file == NULL){
        perror("Error opening file");
        return ERROR;
    }
    // 写入
    int dim = (sizeof (req->y)) / (sizeof (eTPSS *));
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
    mrtree_free_search(req,resp);
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
RESULT mrtree_free(mr_tree * tree){
    // 这个要free掉的太多了，实在是多的数不过来
    return SUCCESS;
}
// 释放resp的节点
RESULT mrtree_free_search(search_req * req,search_resp * resp){
    int dim = sizeof (req->y) / sizeof (eTPSS *);
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
RESULT mrtree_init_origin_node(distance * d,RSQ_data * total,mr_node ** nodes){
    int xl = total->xn;

    int idx = 0;
    if(nodes == NULL)
        return ERROR;
    for(int i = 0 ; i < xl ; ++i){
        mr_node * node =(mr_node *) malloc(sizeof (mr_node));
        if(node == NULL)
            return ERROR;
        // 将heap转换为etpss **
        eTPSS  ** dis = (eTPSS **) malloc(d->d[i]->capacity * sizeof (eTPSS *));
        for(int j = 0 ; j < K_MAX ; ++j){
            dis[j] = d->d[i]->array->key;
        }
        mrtree_init_node(node,total->dim,TRUE,NULL,NULL,total->en_x[i],dis,dis[K_MAX - 1]);
        nodes[idx ++] = node;
    }
    return SUCCESS;
}