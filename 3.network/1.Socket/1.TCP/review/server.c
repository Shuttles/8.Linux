/*************************************************************************
	> File Name: server.c
	> Author: 少年宇
	> Mail: 
	> Created Time: 2020年10月31日 星期六 14时22分34秒
 ************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage : %s port!\n", argv[0]);
        exit(1);
    }

    int port, server_listen;//server_listen作为socket文件描述符
    port = atoi(argv[1]);

    //启动socket
    if ((server_listen = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    printf("Socket create!\n");

    //要门牌号
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = INADDR_ANY;//ip是对方的ip！！！
    if (bind(server_listen, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("bind");
        exit(1);
    }

    printf("Socket bind.\n");

    //接下来要listen
    if (listen(server_listen, 20) < 0) {
        perror("listen");
        exit(1);
    }

    printf("Socket listen.\n");

    while (1) {
        int sockfd;//相当于NPC
        printf("process[%d] : Socket before accept!\n", getpid());

        if ((sockfd = accept(server_listen, NULL, NULL)) < 0) {
            perror("accept");
            close(sockfd);
            continue;
        }

        //printf("Socket after accept!\n\n");

        //使用fork创建子进程
        pid_t pid;
        if ((pid = fork()) < 0) {
            perror("fork");
            continue;
        }

        //因为子进程一开始是复制的父进程的内容，那么他俩可以做相同的事，但是我现在想让他俩做不同的事，就得判断哪个是子进程哪个是父进程
        if (pid == 0) {
            printf("child process[%d]！\n", getpid());
            close(server_listen);//子进程中没必要用了
            char name[20] = {0};
            if (recv(sockfd, name, sizeof(name), 0) <= 0) {
                perror("recv");
                close(sockfd);
            }
            printf("Name = %s\n\n", name);
            exit(0);//如果子进程不在这退出，那么就会继续在while循环里，这不是我们想要的
        }

        close(sockfd);
    }
    return 0;
}
