#include "utils.h"

/*-----------------heap-------------------*/
Heap* createHeap(int capacity,int is_encrypt) {
    Heap* heap = ( Heap* )malloc(sizeof( Heap));
    heap->capacity = capacity;
    heap->size = 0;
    if(is_encrypt == TRUE)
        heap->array = (eTPSS **)malloc(capacity * sizeof(eTPSS *));
    else
        heap->bnArr = (BIGNUM **) malloc(capacity * sizeof (BIGNUM *));
    // 对于每一个array又需要再次进行初始化内部节点
    for(int i = 0 ; i < capacity ; ++i ){
        if(is_encrypt == TRUE){
            heap->array[i] = (eTPSS *) malloc(sizeof (eTPSS));
            init_eTPSS(heap->array[i]);
        }else{
            heap->bnArr[i] = BN_CTX_get(CTX);
        }
    }
    heap->is_encrypt = is_encrypt;
    return heap;
}

void bubleDown(Heap* heap, int index) {
    int largest = index;
    int leftChild = 2*index+1;
    int rightChild = 2*index+2;
    int res = -2;
    while(leftChild < heap->size) {
        if(heap->is_encrypt == TRUE){
            et_Sub(&res,heap->array[leftChild],heap->array[largest]);
            if(leftChild < heap->size && res == 1 ) {
                largest = leftChild;
            }
            et_Sub(&res,heap->array[rightChild], heap->array[largest]);
            if(rightChild < heap->size &&  res == 1 ) {
                largest = rightChild;
            }
            if(largest != index) {
                eTPSS *temp = heap->array[index];
                heap->array[index] = heap->array[largest];
                heap->array[largest] = temp;
                index = largest;
            }else {
                break;
            }

        }else{
            res = BN_cmp(heap->bnArr[leftChild],heap->bnArr[largest]);
            if(leftChild < heap->size && res == -1 ) {
                largest = leftChild;
            }
            res = BN_cmp(heap->bnArr[rightChild],heap->bnArr[largest]);
            if(rightChild < heap->size &&  res == -1 ) {
                largest = rightChild;
            }
            if(largest != index) {
                BIGNUM *temp = heap->bnArr[index];
                heap->bnArr[index] = heap->bnArr[largest];
                heap->bnArr[largest] = temp;
                index = largest;
            }else {
                break;
            }
        }

        leftChild = 2 * index + 1;
        rightChild = 2 * index + 2;

    }

}

void heapify(Heap* heap) {
    for (int i = heap->size/2-1; i >= 0; i--) {
        bubleDown(heap,i);
    }
}

void bubleUp(Heap* heap, int index) {
    int temp = index;
    int parent = (index-1)/2;
    int res = -1;
    if(heap->is_encrypt == TRUE){
        et_Sub(&res,heap->array[parent], heap->array[temp]);
        while(res == 0 && parent >= 0) {
            eTPSS * tempNode = heap->array[temp];
            heap->array[temp] = heap->array[parent];
            heap->array[parent] = tempNode;
            temp = parent;
            parent = (temp-1)/2;
            if(temp == parent)
                break;
            et_Sub(&res,heap->array[parent], heap->array[temp]);
        }
    }else{
        res = BN_cmp(heap->bnArr[parent], heap->bnArr[temp]);
        while(res == 1 && parent >= 0) {
            BIGNUM * tempNode = heap->bnArr[temp];
            heap->bnArr[temp] = heap->bnArr[parent];
            heap->bnArr[parent] = tempNode;
            temp = parent;
            parent = (temp-1)/2;
            if(temp == parent)
                break;
            res = BN_cmp(heap->bnArr[parent], heap->bnArr[temp]);
        }
    }
}

void insert(Heap* heap, void * key) {

    if(heap->size < heap->capacity) {
        heap->size++;
        if(heap->is_encrypt == TRUE){
            // 直接引用
            et_Copy(heap->array[heap->size-1],key);
        }else{
            BN_copy(heap->bnArr[heap->size -1],key);
        }
        bubleUp(heap,heap->size-1);
    }
}

void deleteMax(Heap* heap) {
    void * temp;
    if(heap->size >= 1) {
        if(heap->is_encrypt == TRUE){
            temp = heap->array[0];
            heap->array[0] = heap->array[heap->size-1];
            heap->array[heap->size-1] = temp;
            heap->size--;
            bubleDown(heap,0);
        }else{
            temp = heap->bnArr[0];
            heap->bnArr[0] = heap->bnArr[heap->size-1];
            heap->bnArr[heap->size-1] = temp;
            heap->size--;
            bubleDown(heap,0);
        }
    }
}

void heapSort(Heap* heap) {
    int temp = heap->size;

    for (int i = 0; i <temp ; ++i) {
        deleteMax(heap);
    }
    heap->size = temp;
}

/*-----------------------------------------------*/
void * bignum_sqrt(BIGNUM  * res,BIGNUM* num) {
    BIGNUM* sqrt_result = BN_new();
    BIGNUM* temp = BN_new();
    BIGNUM* remainder = BN_new();
    BIGNUM* quotient = BN_new();
    BIGNUM* two = BN_new();

    BN_set_word(two, 2);
    BN_copy(temp, num);

    // 使用二分法进行开平方计算
    while (BN_cmp(temp, sqrt_result) > 0) {
        BN_copy(sqrt_result, temp);
        BN_div(quotient, remainder, num, sqrt_result, BN_CTX_new());
        BN_add(temp, sqrt_result, quotient);
        BN_rshift1(temp, temp);
    }

    BN_free(temp);
    BN_free(remainder);
    BN_free(quotient);
    BN_free(two);
    return sqrt_result;
}

void printDebugInfo(BIGNUM * res,eTPSS * et,const char * funcName,int line,char * paramName){
    if(res == NULL && et == NULL){
        fprintf(stderr,"%s[%d]:传入参数不符合\n",funcName,line );
        goto end;
    }
    if(res != NULL && et != NULL){
        fprintf(stderr,"%s[%d]:传入参数不符合\n",funcName,line );
        goto end;
    }
    if(res != NULL){
        char * str = BN_bn2dec(res);
        fprintf(stdout,"%s[%d]: %s的值为-->%s\n",funcName,line,paramName,str);
    }else{
        BIGNUM * tmp = BN_CTX_get(CTX);
        et_Recover(tmp,et);
        char * str = BN_bn2dec(tmp);
        fprintf(stdout,"%s[%d]: %s的值为-->%s\n",funcName,line,paramName,str);
        BN_clear(tmp);
    }

    end:
    fflush(stdout);
    fflush(stderr);
}


// 清空heap
void heapClear(Heap * heap){
    // 还就是这么简单，然后继续填充值就行
    heap->size = 0;
}
// 弹出前几个值
void heap_PopK_max_Val(Heap * heap,int k_max,eTPSS ** arr){
    for(int i = 0 ; i < k_max ; ++i){
        // 最小值弹向队尾
        deleteMax(heap);
        eTPSS * tt = (eTPSS *) malloc(sizeof (eTPSS));
        init_eTPSS(tt);
        if(heap->is_encrypt == TRUE){
            et_Copy(tt,heap->array[heap->size]);
        }else{
            et_Share(tt,heap->bnArr[heap->size]);
        }
        arr[i] = tt;
    }
}
// 释放heap的内存
void heap_free(Heap * h,int y_len){
    // 对于堆内存的释放
    if(h->is_encrypt == TRUE){
        for(int i = 0 ; i < y_len ; ++i) {
            free_eTPSS(h->array[i]);
            free(h->array[i]);
        }
    }else{
        for(int i = 0 ; i < y_len ; ++i) {
            BN_clear(h->bnArr[i]);
        }
    }

    if(h->is_encrypt == TRUE){
        free(h->array);
    }else{
        free(h->bnArr);
    }
    free(h);
}