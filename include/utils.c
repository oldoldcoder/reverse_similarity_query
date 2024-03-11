#include "utils.h"

/*-----------------heap-------------------*/
Heap* createHeap(int capacity) {
    Heap* heap = ( Heap* )malloc(sizeof( Heap));
    heap->capacity = capacity;
    heap->size = 0;
    heap->array = (Node*)malloc(capacity * sizeof(Node));
    // 对于每一个array又需要再次进行初始化内部节点
    for(int i = 0 ; i < capacity ; ++i ){
        heap->array[i].key = (eTPSS *) malloc(sizeof (eTPSS));
        init_eTPSS(heap->array[i].key);
    }
    return heap;
}

void printHeap(Heap* heap) {
    char * str = NULL;
    BIGNUM * t = BN_CTX_get(CTX);
    for (int i = 0; i < heap->size; ++i) {

        et_Recover(t,heap->array[i].key);
        str = BN_bn2dec(t);
        printf("%s ", str);
    }
    BN_clear(t);
    printf("\n");
}

void bubleDown(Heap* heap, int index) {
    int largest = index;
    int leftChild = 2*index+1;
    int rightChild = 2*index+2;
    int res = -1;
    while(leftChild < heap->size) {
        et_Sub(&res,heap->array[leftChild].key,heap->array[largest].key);
        if(leftChild < heap->size && res == 1 ) {
            largest = leftChild;
        }
        et_Sub(&res,heap->array[rightChild].key, heap->array[largest].key);
        if(rightChild < heap->size &&  res == 1) {
            largest = rightChild;
        }
        if(largest != index) {
            Node temp = heap->array[index];
            heap->array[index] = heap->array[largest];
            heap->array[largest] = temp;
            index = largest;
        }else {
            break;
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
    et_Sub(&res,heap->array[parent].key, heap->array[temp].key);
    while(res == 0 && parent >= 0) {
        Node tempNode = heap->array[temp];
        heap->array[temp] = heap->array[parent];
        heap->array[parent] = tempNode;
        temp = parent;
        parent = (temp-1)/2;
    }
}

void insert(Heap* heap, eTPSS * key) {

    if(heap->size < heap->capacity) {
        heap->size++;
        // 直接引用
        heap->array[heap->size-1].key = key;
        bubleUp(heap,heap->size-1);
    }
}

void deleteMax(Heap* heap) {
    struct Node temp;
    if(heap->size>1) {
        temp = heap->array[0];
        heap->array[0] = heap->array[heap->size-1];
        heap->array[heap->size-1] = temp;
        heap->size--;
        bubleDown(heap,0);
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

