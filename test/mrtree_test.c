/**
@author:heqi
@time:2024/03/111
@desc:验证数据的测试文件
*/

#include "utils.h"
#include "mrtree.h"
#include "RSQ_data_structure.h"
#include "eTPSS.h"
#include "stdio.h"

// 关联头文件的内容，书写测试的文件
int main(){
    // 读取文件
    RSQ_data total;
    initialize_Constant();
    RSQ_read_data(&total);
    // 对数据进行加密
    RSQ_encrypt_setx(&total);
    distance d;
    mrtree_init_distance(&d,total.xn,total.yn);
    // 进行排序
    mrtree_compute_xy_distance(&d,&total);
    // 打印我们heap里面点的距离all right
    for(int i = 0 ; i < d.x_len ; i ++){
        printf("第%d个数的堆结构：",i);
        for(int j = 0 ; j < d.x_len ; j++){
            printDebugInfo(NULL,d.d[i]->array[j],__func__ ,__LINE__,"堆里面的数据");
        }
        printf("\n");
    }
    // 初始化最初的节点
    mr_node ** nodes = (mr_node **) malloc(total.xn * sizeof (mr_node *));

    mrtree_init_origin_node(&d,&total,nodes);

    // 创建树
    mr_tree  tree;
    tree.root = mrtree_create_tree(nodes,total.xn);
    int k = 2;
    search_req req;
    search_resp  resp;
    // 初始化查询
    mrtree_init_query_param(&req,&resp,k,total.open_y[3]);
    // 进行查询
    mrtree_search(&tree,&req,&resp);
    mrtree_write_resp(&req,&resp,total.dim);

    // 释放被占用的空间
    RSQ_free(&total);
    mrtree_free(&tree);
    return 0;
}
