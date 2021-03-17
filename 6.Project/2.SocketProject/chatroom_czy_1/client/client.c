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

char *conf = "./client.conf";

int main() {
    int port, sockfd;
    char ip[20] = {0};
    struct Msg msg;
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
    

    return 0;
}
