/*************************************************************************
	> File Name: client.c
	> Author: 少年宇
	> Mail: 
	> Created Time: 2021年01月14日 星期四 22时39分52秒
 ************************************************************************/

#include "head.h" 
#include "common.h"

int socket_connect(char *host, int port) {
    int sockfd;
    struct sockaddr_in server;

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(host);

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        return -1;
    }
    printf("Socket create.\n");

    if (connect(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("connect");
        return -1;
    }

    return sockfd;
}

//利用select完成带有超时时间的socket_connect
//1.设置sockfd为非阻塞
//2.利用select的阻塞完成超时返回功能
int socket_connect_timeout(char *host, int port, long timeout) {
    int sockfd;
    struct sockaddr_in server;

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(host);

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        return -1;
    }


    make_nonblock(sockfd);//设置成非阻塞
    fd_set wfds;//监听是否可写
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = timeout;

    FD_ZERO(&wfds);
    FD_SET(sockfd, &wfds);

    if (connect(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        int retval = 0;
        retval = select(sockfd + 1, NULL, &wfds, NULL, &tv);//不在connect处阻塞，而在这里阻塞

        if (retval < 0) {
            return -1;
        } else if (retval) {
            //ok
            //待完成
        } else {
            //retval == 0 ->timeout
            return -1;
        }
    }

    make_block(sockfd);
    return sockfd;
}

