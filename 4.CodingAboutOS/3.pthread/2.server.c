/*************************************************************************
	> File Name: 2.server.c
	> Author: 少
	> Mail: 
	> Created Time: 2020年03月28日 星期六 10时12分32秒
 ************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

int retval = 3;

void *receive(void *arg) {
    int sockfd = *(int *)arg;
    
    char name[20] = {0};
    if ((recv(sockfd, name, sizeof(name), 0)) <= 0) {
        perror("recv");
        close(sockfd);
        retval = 4;
        pthread_exit((void *)&retval);
    }
    printf("Client name = %s\n", name);

    pthread_exit((void *)&retval);
    return NULL;
}


int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage : %s port \n", argv[0]);
        exit(1);
    }

    int port, server_listen;
    port = atoi(argv[1]);//将端口号字符串转化为整形

    if ((server_listen = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    printf("Socket create.\n");
    struct sockaddr_in server;
    server.sin_family = AF_INET;//ipv4
    server.sin_port = htons(port);//转换为网络字节序
    server.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_listen, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("bind");
        exit(1);
        //问大爷要了门牌号
    }
    //等别人连我

    printf("Socket bind.\n");

    if (listen(server_listen, 20) < 0) {
        perror("server");
        exit(1);
    }

    while (1) {
        printf("Socket before accept.\n");
        int sockfd;
        if ((sockfd = accept(server_listen, NULL, NULL)) < 0) {
            //表示不关注对方的ip
            perror("accept");
            close(sockfd);
            continue;
        }

        pthread_t tid;
        void *status;
        
        pthread_create(&tid, NULL, receive, (void *)&sockfd);
        pthread_join(tid, &status);
    }
    

    return 0;
}
