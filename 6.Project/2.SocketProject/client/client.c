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

char *conf = "./client.conf";

int main(int argc, char **argv) {
    int port, sockfd;
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

    return 0;
}
