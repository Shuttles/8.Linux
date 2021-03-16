/*************************************************************************
	> File Name: client.c
	> Author: 少年宇
	> Mail: 
	> Created Time: 2021年01月16日 星期六 17时01分49秒
 ************************************************************************/

#include <../common/head.h>
#include <../common/tcp_client.h>
#include <../common/chatroom.h>
#include <../common/common.h>
#include <../common/color.h>

char *conf = "./client.conf";

int sockfd;
char logfile[50] = {0};

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
    strcpy(logfile, get_value(conf, "LOG_FILE"));
    
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
        sleep(2);
        signal(SIGINT, logout);
        //子进程发送消息
        system("clear");
        while (1) {
            printf(PINK_HL("Please Input Message:\n"));
            scanf("%[^\n]s", msg.message);
            getchar();//吞回车
            msg.flag = 0;

            if (msg.message[0] == '@') {
                msg.flag = 1;
            }
            chat_send(msg, sockfd);
            memset(msg.message, 0, sizeof(msg.message));
            system("clear");
        }
        close(sockfd);
    } else {
        //父进程接收消息
        FILE *log_fp = fopen(logfile, "w");
        struct RecvMsg rmsg;
        while (1) {
            rmsg = chat_recv(sockfd);

            if (rmsg.retval < 0) {
                printf(RED_HL("Error in Server!\n"));
                break;
            }

            if (rmsg.msg.flag == 0) {
                //公聊信息
                fprintf(log_fp, BLUE_HL("%s") " : %s\n", rmsg.msg.from, rmsg.msg.message);
                printf(BLUE_HL("%s") " : %s\n", rmsg.msg.from, rmsg.msg.message);
            } else if (rmsg.msg.flag == 2) {
                fprintf(log_fp, YELLOW_HL("通知信息") " : %s\n", rmsg.msg.message);
                printf(YELLOW_HL("通知信息") " : %s\n", rmsg.msg.message);
            } else if (rmsg.msg.flag == 1) {
                //私聊信息
                fprintf(log_fp, GREEN_HL("私聊信息，from ") BLUE_HL("%s") " : %s\n", rmsg.msg.from, rmsg.msg.message);
                printf(GREEN_HL("私聊信息，from ") BLUE_HL("%s") " : %s\n", rmsg.msg.from, rmsg.msg.message);
            } else {
                printf("Error!\n");
            }

            fflush(log_fp);
        }
        
        fclose(log_fp);
        wait(NULL);//等待子进程
        close(sockfd);
    }

    return 0;
}
