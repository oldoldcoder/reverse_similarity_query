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
void printfTime(char* desc,clock_t start) {
    double cpu_time_used = ((double)(clock() - start)) / CLOCKS_PER_SEC;

    printf("%s used: %f seconds\n",desc, cpu_time_used);
}
//// 关联头文件的内容，书写测试的文件
int main(){
    // 初始化函数的内容
    RSQ_data data;
    mr_tree tree;
    clock_t start, end;
    start = clock();

    init_algo("/root/heqi/encryption_algorithm/reverse_similarity_query/data/RSQ_DATA_FILE.txt", &data, &tree);
    printfTime("init ", start);
    // 进行查询
    start = clock();
    query_algo(&data, &tree, "/root/heqi/encryption_algorithm/reverse_similarity_query/data/REQ_DATA_FILE.txt", "/root/heqi/encryption_algorithm/reverse_similarity_query/data/RESP_DATA_FILE.txt");
    printfTime("query ", start);
    // 释放内容 TODO 树的内容还没有释放
    free_algo(&data, &tree);

    return 0;
}
