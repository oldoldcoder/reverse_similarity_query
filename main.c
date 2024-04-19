/**
@author:heqi
@time:2024/03/11
@desc:验证数据的测试文件
*/

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "unistd.h"
#include "arpa/inet.h"
#include "RSQ_data_structure.h"
#include "mrtree.h"
#include "utils.h"


#define PORT 9991
#define MAX_BUFFER_SIZE 1024

#define HANDLE_OK "ok\n"
#define HANDLE_ERROR "error\n"
#define status_restart 1
#define status_stop (-1)

#define STOP "stop"
#define INIT "init"
#define QUERY "query"


/*-----------------全局变量-------------------*/
u_char isInit = 0;
RSQ_data  total;
Heap * heap;
eTPSS  *** dis;
mr_tree tree;

// 目前数据的总量
int n;
// 目前数据的维度
int dim;
/*-----------------------------------------*/
int handle_message(int clientSocket){
    char buffer[MAX_BUFFER_SIZE];
    while(1){
        // 清空内容准备下一次读取
        memset(buffer,0,MAX_BUFFER_SIZE);
        if(recv(clientSocket,buffer,MAX_BUFFER_SIZE,0) <= 0){
            perror("Receiving data failed");
            return status_restart;
        }
        printf("Message form Java Client: %s \n",buffer);
        // 处理接受的消息
        if(strcmp(buffer,STOP) == 0){
            send(clientSocket,"stopped\n", strlen("stopped\n"),0);
            return status_stop;
        }
        char * handle = HANDLE_OK;
        // 初始化数据
        if(strcmp(buffer,INIT) == 0){
            // 如果已经初始化，删除之前初始化的数据
            if(isInit == 1){
                // 清除堆的内容
                heap_free(heap,total.yn);
                // 调用清除方法释放内存
                RSQ_free(&total);
                // 清除树
                mrtree_free(&tree);
                // TODO 清除distance的内容

            }
            // 重新读取重新构建
            if(RSQ_read_data(&total) != SUCCESS){
                printf("读取数据失败\n");
                handle = HANDLE_ERROR;
                goto send_msg;
            }
            if(RSQ_encrypt_setx(&total) != SUCCESS){
                printf("加密数据失败\n");
                handle = HANDLE_ERROR;
                goto send_msg;
            }
            n = total.xn;
            dim = total.dim;
            dis = (eTPSS ***) malloc(total.xn * sizeof (eTPSS **));
            if(mrtree_init_distance(dis,&heap,total.xn,total.yn) != SUCCESS){
                printf("初始化距离数组失败\n");
                handle = HANDLE_ERROR;
                goto send_msg;
            }

            if(mrtree_compute_xy_distance(heap,&total,dis) != SUCCESS){
                printf("计算x集合中每个点与y集合中点的距离失败\n");
                handle = HANDLE_ERROR;
                goto send_msg;
            }
            // 创建这个树的根节点
            mr_node ** nodes = (mr_node **) malloc(total.xn * sizeof (mr_node *));

            if(mrtree_init_origin_node(dis,&total,nodes) != SUCCESS){
                printf("初始化根节点数据失败\n");
                handle = HANDLE_ERROR;
                goto send_msg;
            }
            tree.root = mrtree_create_tree(nodes,total.xn);
            if(tree.root == NULL){
                printf("创建树失败\n");
                handle = HANDLE_ERROR;
                goto send_msg;
            }
            isInit = 1;
            printf("初始化反向查询的结构成功\n");
        }

        // 查询,每次查询完毕释放内存
        if(strcmp(buffer,QUERY) == 0){
            // 查询内容，写入文件
            search_req  req;
            search_resp resp;
            // 查询的数自己定义
            if(mrtree_init_query_param(&req,&resp,total.dim)!= SUCCESS){
                printf("初始化查询参数失败\n");
                handle = HANDLE_ERROR;
                goto send_msg;
            }
            // 查询
            if(mrtree_search(&tree,&req,&resp) != SUCCESS){
                printf("查询内容失败\n");
                handle = HANDLE_ERROR;
                goto send_msg;
            }
            // 结果文件写入操作
            if(mrtree_write_resp(&req,&resp,total.dim) != SUCCESS){
                printf("写入文件失败\n");
                handle = HANDLE_ERROR;
                goto send_msg;
            }
            // 释放内存
            mrtree_free_search(&req,&resp,dim);
        }
send_msg:
        send(clientSocket,handle, strlen(handle),0);
        fflush(stdout);
    }
}

int main(){

    // 初始化全局的变量
    initialize_Constant();

    int serverSocket,clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrLen = sizeof(clientAddr);

    // 创建socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // 设置服务器地址结构
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(PORT);

    // 绑定地址和端口
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Binding failed");
        exit(EXIT_FAILURE);
    }

    // 监听连接
    if (listen(serverSocket, 1) < 0) { // 只允许一个等待连接
        perror("Listening failed");
        exit(EXIT_FAILURE);
    }

    // 接受连接
    while (1) {
        // 接受连接
        clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &addrLen);
        if (clientSocket < 0) {
            perror("Accepting connection failed");
            exit(EXIT_FAILURE);
        }

        // 处理消息
        if(handle_message(clientSocket) == status_stop)
            break;
        // 关闭连接
        close(clientSocket);
    }
    close(serverSocket);

    return 0;
}

