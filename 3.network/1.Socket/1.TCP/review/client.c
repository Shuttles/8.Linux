/*************************************************************************
	> File Name: client.c
	> Author: 少年宇
	> Mail: 
	> Created Time: 2020年10月31日 星期六 14时40分11秒
 ************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]) {
    int sockfd, port;
    struct sockaddr_in server;
    if (argc != 3) {
        fprintf(stderr, "Usage : %s ip port!\n", argv[0]);
        exit(1);
    }

    port = atoi(argv[2]);

    //对server结构体的处理
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(argv[1]);

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    printf("Socket create.\n");


    //主动连接
    if (connect(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("connect");
        exit(1);
    }

    //发送数据，为一行字符串
    if (send(sockfd, "王家龙太强了", sizeof("王家龙太强了"), 0) < 0) {
        perror("send");
        exit(1);
    }
    
    printf("发送名字成功！\n");

    close(sockfd);
    return 0;
}
