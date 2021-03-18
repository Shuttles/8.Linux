/*************************************************************************
	> File Name: client.c
	> Author: 少年宇
	> Mail: 
	> Created Time: 2021年03月17日 星期三 14时19分37秒
 ************************************************************************/

#include <head.h>
#include <tcp_client.h>
#include <chatroom.h>
#include <common.h>
#include <color.h>

char *conf = "./client.conf";
int sockfd;

void logout(int signalnum) {
    close(sockfd);
    printf(RED_HL("您已退出！\n"));
    exit(1);
    return ;
}

int main() {
    int port;
    char ip[20] = {0};
    struct Msg msg;
    port = atoi(get_value(conf, "SERVER_PORT"));
    strcpy(ip, get_value(conf, "SERVER_IP"));

    if ((sockfd = socket_connect(ip, port)) < 0) {
        perror("socket_connect");
        return 1;
    }

    //连上server之后先告诉server我的名字，并请求进入聊天室
    strcpy(msg.from, get_value(conf, "MY_NAME"));
    msg.flag = 2;
    if (chat_send(msg, sockfd) < 0) {
        return 2;
    }

    //接受server的反馈，看其是否让我进入聊天室
    //if flag == 3, 就是不让我进入
    struct RecvMsg rmsg = chat_recv(sockfd);
    if (rmsg.retval < 0) {
        fprintf(stderr, RED_HL("Server Error!\n"));
        return 1;
    }

    printf(GREEN("[Server]: ") "%s\n", rmsg.msg.message);
    
    if (rmsg.msg.flag == 3) {
        close(sockfd);
        return 3;
    }


    pid_t pid;
    if ((pid = fork()) < 0) {
        perror("fork");
        return 4;
    }

    if (pid == 0) {
        signal(SIGINT, logout);
        system("clear");
        while (1) {
            printf(PURPLE_HL("Please Input Message\n"));
            memset(msg.message, 0, sizeof(msg.message));
            scanf("%[^\n]s", msg.message);
            getchar();
            chat_send(msg, sockfd);
            system("clear");
        }
    } else {
        wait(NULL);
        close(sockfd);
    }
    return 0;
}
