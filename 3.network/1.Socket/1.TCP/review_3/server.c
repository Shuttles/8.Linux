/*************************************************************************
	> File Name: server.c
	> Author: 少年宇
	> Mail: 
	> Created Time: 2021年03月17日 星期三 10时25分09秒
 ************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage : %s port\n", argv[0]);
        exit(1);
    }

    int port, server_listen;
    port = atoi(argv[1]);

    //启动socket
    if ((server_listen = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    printf("Socket_create.\n");

    //bind
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = INADDR_ANY;
    if (bind(server_listen, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("bind");
        exit(1);
    }

    printf("Socket_bind.\n");

    //listen
    if (listen(server_listen, 20) < 0) {
        perror("server");
        exit(1);
    }

    while (1) {
        int sockfd;
        printf("Socket before accept.\n");
        if ((sockfd = accept(server_listen, 0, 0)) < 0) {
            perror("accept");
            close(sockfd);
            continue;
        }

        printf("Socket After accept\n");

        pid_t pid;
        if ((pid = fork()) < 0) {
            perror("fork");
            continue;
        }

        if (pid == 0) {
            close(server_listen);
            char name[20] = {0};
            if (recv(sockfd, name, sizeof(name), 0) <= 0) {
                perror("recv");
                close(sockfd);
            }
            printf("Name = %s\n\n", name);
            exit(0);
        }



        /*
        char name[20] = {0};
        if (recv(sockfd, name, sizeof(name), 0) <= 0) {
            close(sockfd);
            continue;
        }

        printf("Socket received\n");
        printf("name : %s\n\n", name);
        close(sockfd);
        */
    }



    return 0;
}
