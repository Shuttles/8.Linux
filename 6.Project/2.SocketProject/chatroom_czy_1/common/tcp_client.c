/*************************************************************************
	> File Name: client.c
	> Author: 少年宇
	> Mail: 
	> Created Time: 2021年03月17日 星期三 11时21分16秒
 ************************************************************************/

#include "head.h"

int socket_connect(char *host, int port) {
    int sockfd;
    struct sockaddr_in server;

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(host);

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        //perror("socket");
        return -1;
    }

    printf("Socket create.\n");

    if (connect(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        //perror("connect");
        return -1;
    }

    return sockfd;
}
