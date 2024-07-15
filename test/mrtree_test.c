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
int main(){
    // 初始化函数的内容
    RSQ_data data;
    mr_tree tree;
    init_algo("D:\\study\\code\\ClionProject\\reverse_similarity_query\\data\\RSQ_DATA_FILE.txt", &data, &tree);
    // 进行查询
    query_algo(&data, &tree, "D:\\study\\code\\ClionProject\\reverse_similarity_query\\data\\REQ_DATA_FILE.txt", "D:\\study\\code\\ClionProject\\reverse_similarity_query\\data\\RESP_DATA_FILE.txt");
    // 释放内容 TODO 树的内容还没有释放
    free_algo(&data, &tree);

    return 0;
}
