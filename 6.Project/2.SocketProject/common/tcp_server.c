/*************************************************************************
	> File Name: server.c
	> Author: 少年宇
	> Mail: 
	> Created Time: 2021年01月14日 星期四 22时27分01秒
 ************************************************************************/

#include "head.h" 

int socket_create(int port) {//创建监听状态的socket
    int server_listen;

    if ((server_listen = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        return -1;
    }

    printf("Socket create.\n");
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_listen, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("bind");
        return -1;
    }

    printf("Socket bind.\n");
    if (listen(server_listen, 20) < 0) {
        perror("listen");
        return -1;
    }

    return server_listen;
}
