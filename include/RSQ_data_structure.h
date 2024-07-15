/**
@author:heqi
@time:2024/03/08
@desc:定义反向查询KNN算法的数据结构
*/
#ifndef RSQ_DATA_STRUCTURE_H
#define RSQ_DATA_STRUCTURE_H

#include <openssl/bn.h>
#include "eTPSS.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "utils.h"

/*---------------常量的定义---------------*/
#define RSQ_DATA_FILE "/root/heqi/encryption_algorithm/reverse_similarity_query/data/data.txt"
#define REQ_DATA_PATH "/root/heqi/encryption_algorithm/reverse_similarity_query/data/query.txt"
#define RESP_DATA_PATH "/root/heqi/encryption_algorithm/reverse_similarity_query/data/res.txt"
// 定义KMAX的量,在初始化的时候赋值
extern int K_MAX;
/*---------------数据结构的定义---------------*/
// 普通的一个单体数据的定义，内部的维度是d，拥有d维的data数据组
typedef struct {
    int dim;
    BIGNUM ** single_data;
}set_y;
// 对于普通数据的加密，使用第三方秘密共享加密
typedef struct {
    int dim;
    eTPSS ** en_data;
    BIGNUM  ** de_data;
} set_x;
// 对于所有数据的定义
typedef struct {
    int xn;// 加密数据x的总数
    int yn;// 非加密数据y的总数
    int dim; // 维度是相同的
    set_x ** en_x;
    set_y ** open_y;
}RSQ_data;

/*--------------函数过程的定义---------------*/
// 从文件读取数据填充两个set
RESULT RSQ_read_data(RSQ_data * data,char * dataFilePath);
// 对于x数据的解密
RESULT RSQ_decrypt_setx(RSQ_data * data);
// 对于x数据的加密
RESULT RSQ_encrypt_setx(RSQ_data * data);
// 释放做占用的空间数据
RESULT RSQ_free(RSQ_data * data);
#endif //RSQ_DATA_STRUCTURE_H