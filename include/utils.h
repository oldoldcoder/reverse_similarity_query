#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <malloc.h>
#include <time.h>
#include "openssl//bn.h"
#include "eTPSS.h"
/*---------------定义常量------------*/
#define SUCCESS 2147483617
#define ERROR 2147483629
#define TRUE 1
#define FALSE 0
#define RESULT int
/*--------------堆的实现----------------*/

typedef struct Heap {
    eTPSS **array;
    int size;
    int capacity;
} Heap;

Heap* createHeap(int capacity);
void printHeap(Heap* heap);
void bubleDown(Heap* heap, int index);
void heapify(Heap* heap);
void bubleUp(Heap* heap, int index);
void insert(Heap* heap, eTPSS * key);
void deleteMax(Heap* heap);
void heapSort(Heap* heap);
/*--------------------------------*/

void * bignum_sqrt(BIGNUM  * res ,BIGNUM* num);
// 打印调试信息
void printDebugInfo(BIGNUM * res,eTPSS * et,const char * funcName,int line,char * paramName);
// 打印查看一个node节点的所有值

#endif // UTILS_H