/**
@author:heqi
@time:2024/03/08
@desc:定义反向查询KNN算法的MR树结构
*/
#ifndef MRTREE_H
#define MRTREE_H

#include "utils.h"
#include <openssl/bn.h>
#include "eTPSS.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "RSQ_data_structure.h"


/*-------------------结构定义------------------*/

// 抽象结构,构建mr的树的时候使用的数据
typedef struct mr_node{
    // 数据
    set_x * data;
    // 是否是叶子节点
    int is_left_ndoe;
    // 表示所圈范围的队列，2维度n行
    eTPSS *** range;
    // 距离的点
    eTPSS ** distance;
    // 距离最大的点
    eTPSS * maxDistance;
    // dim
    int dim;
    // 左右节点
    struct mr_node * left;
    struct mr_node * right;

}mr_node;

// MR 树的数据结构
typedef struct mr_tree{
    // mr树的root根
    mr_node  * root;
} mr_tree;

// 定义一个查询请求
typedef struct search_req{
    // 定义k近邻的k
    int k;
    eTPSS ** y;
} search_req;

typedef struct search_res_node{
    set_x * data;
    struct search_res_node *next;
}res_node;

typedef struct search_resp{
    //查询的结果链表
    res_node * root;
    res_node * now;
} search_resp;

/**--------------------JNA调用函数-------------------*/
EXPORT_SYMBOL RESULT init_algo(char* dataFilePath, RSQ_data* data, mr_tree* tree);
EXPORT_SYMBOL RESULT query_algo(RSQ_data* data, mr_tree* tree, char* queryFilePath, char* resultFilePath);
EXPORT_SYMBOL RESULT free_algo(RSQ_data* data, mr_tree* tree);
/*------------------方法定义------------------*/
// 初始化distance
RESULT mrtree_init_distance(eTPSS ***dis,Heap ** heap,int x_len,int y_len);
// 计算距离,完毕之后会使用sort方法对于数据进行排序
RESULT mrtree_compute_xy_distance(Heap * d,RSQ_data * data,eTPSS *** dis);
// 计算不同的x之间的距离
RESULT mrtree_compute_inner_distance(int * map,mr_node ** nodes,int size);
// 创建MR树的过程，返回的是根节点，自底向上构建树
mr_node * mrtree_create_tree(mr_node ** nodes,int size);
// 进行反向查询
RESULT mrtree_search(mr_tree * tree,search_req * req, search_resp * resp);
// 初始化查询以及我们的结果
RESULT mrtree_init_query_param(search_req * req, search_resp * resp,int dim,char* queryFilePath);
// 查询结果的写入
RESULT mrtree_write_resp(search_req * req, search_resp * resp,int dim,char * resultPath);
// 初始化一个叶节点
RESULT mrtree_init_node(mr_node * node,int dim,int is_left, mr_node * right, mr_node * left,set_x * data,eTPSS ** distance, eTPSS * maxDistance);

// 释放内存的点
RESULT mrtree_free(mr_node* root);
// 释放resp的节点
RESULT mrtree_free_search(search_req * req,search_resp * resp,int dim);
// 初始化最初的节点
RESULT mrtree_init_origin_node(eTPSS *** dis,RSQ_data * total,mr_node ** nodes);
// 清除查询的时候申请的内存
RESULT mrtree_free_search_param(search_req * req,search_resp * resp,int dim);
void printShowNodeVal(mr_node **nodes,int);
#endif// MRTREE_H