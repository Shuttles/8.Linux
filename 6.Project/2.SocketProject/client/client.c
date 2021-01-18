/*************************************************************************
	> File Name: client.c
	> Author: 少年宇
	> Mail: 
	> Created Time: 2021年01月16日 星期六 17时01分49秒
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
    printf("您已退出！\n");
    exit(1);
    return ;
}


int main(int argc, char **argv) {
    int port;
    struct Msg msg;
    char ip[20] = {0};
    port = atoi(get_value(conf, "SERVER_PORT"));
    strcpy(ip, get_value(conf, "SERVER_IP"));
    
    if ((sockfd = socket_connect(ip, port)) < 0) {
        perror("socket_connect");
        return 1;
    }
    
    strcpy(msg.from, get_value(conf, "MY_NAME"));
    msg.flag = 2;

    if (chat_send(msg, sockfd) < 0) {
        return 2;
    } 

    struct RecvMsg rmsg = chat_recv(sockfd);//我必须知道server是拒绝我的连接还是接收连接

    if (rmsg.retval < 0) {
        fprintf(stderr, "Error!\n");
        return 1;
    }

    printf(GREEN("Server") " : %s\n", rmsg.msg.message);

    if (rmsg.msg.flag == 3) {
        close(sockfd);
        return 1;
    }

    pid_t pid;
    if ((pid = fork()) < 0) {
        perror("fork");
    }


    if (pid == 0) {
        signal(SIGINT, logout);
        //子进程发送消息
        system("clear");
        while (1) {
            printf(PINK_HL("Please Input Message:\n"));
            scanf("%[^\n]s", msg.message);
            getchar();//吞回车
            chat_send(msg, sockfd);
            memset(msg.message, 0, sizeof(msg.message));
            system("clear");
        }
    } else {
        //父进程接收消息
        wait(NULL);//等待子进程
        close(sockfd);
    }

    return 0;
}
