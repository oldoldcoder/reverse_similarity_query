/**
@author:heqi
@time:2024/03/11
@desc:验证数据的测试文件
*/

#include "utils.h"
#include "mrtree.h"
#include "RSQ_data_structure.h"
#include "eTPSS.h"
#include "stdio.h"

//// 关联头文件的内容，书写测试的文件
//int main(){
//    // 读取文件
//    RSQ_data total;
//    initialize_Constant();
//    clock_t start_time = clock();
//    RSQ_read_data(&total);
//    clock_t end_time = clock();
//    double execution_time = ((double) (end_time - start_time)) / CLOCKS_PER_SEC * 1000;
//    printf("读取数据速度：%f 毫秒\n", execution_time);
//    fflush(stdout);
//    // 对数据进行加密
//    start_time = clock();
//    RSQ_encrypt_setx(&total);
//    end_time = clock();
//    execution_time = ((double) (end_time - start_time)) / CLOCKS_PER_SEC * 1000;
//    printf("加密速度：%f 毫秒\n", execution_time);
//    fflush(stdout);
//
//    // RSQ_decrypt_setx(&total);
//
//    /*-------------------创建树阶段--------------------*/
//    Heap * heap;
//    eTPSS  *** dis = (eTPSS ***) malloc(total.xn * sizeof (eTPSS **));
//    // 初始化我们的dis的内存空间
//    mrtree_init_distance(dis,&heap,total.xn,total.yn);
//    // 进行排序
//    start_time = clock();
//    mrtree_compute_xy_distance(heap,&total,dis);
//    end_time = clock();
//    execution_time = ((double) (end_time - start_time)) / CLOCKS_PER_SEC * 1000;
//    printf("计算距离速度：%f 毫秒\n", execution_time);
//    fflush(stdout);
//
//    // 打印我们heap里面点的距离all right
//    /*for(int i = 0 ; i < d.x_len ; i ++){
//        printf("第%d个数的堆结构：",i);
//        for(int j = 0 ; j < d.x_len ; j++){
//            printDebugInfo(NULL,d.d[i]->array[j],__func__ ,__LINE__,"堆里面的数据");
//        }
//        printf("\n");
//    }*/
//    // 初始化最初的节点
//    mr_node ** nodes = (mr_node **) malloc(total.xn * sizeof (mr_node *));
//    start_time = clock();
//    mrtree_init_origin_node(dis,&total,nodes);
//
//    // 创建树
//    mr_tree  tree;
//    tree.root = mrtree_create_tree(nodes,total.xn);
//    end_time = clock();
//    execution_time = ((double) (end_time - start_time)) / CLOCKS_PER_SEC * 1000;
//    printf("创建树速度：%f 毫秒\n", execution_time);
//    fflush(stdout);
//    int k = 20;
//    search_req req;
//    search_resp  resp;
//    start_time = clock();
//
//
//
//
//    // 初始化查询
//    mrtree_init_query_param(&req,&resp,k,total.open_y[3]);
//    // 进行查询
//    mrtree_search(&tree,&req,&resp);
//    end_time = clock();
//    execution_time = ((double) (end_time - start_time)) / CLOCKS_PER_SEC * 1000;
//    printf("查询速度：%f 毫秒\n", execution_time);
//    fflush(stdout);
//    mrtree_write_resp(&req,&resp,total.dim);
//
//    // 释放被占用的空间
//    RSQ_free(&total);
//    mrtree_free(&tree);
//    return 0;
//}
