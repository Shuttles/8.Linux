/*************************************************************************
	> File Name: server.c
	> Author: 少年宇
	> Mail: 
	> Created Time: 2021年01月16日 星期六 16时54分01秒
 ************************************************************************/

#include "../common/head.h"
#include "../common/tcp_server.h"
#include "../common/common.h"
#include "../common/chatroom.h"

struct User {
    char name[20];
    int online;
    pthread_t tid;
    int fd;
};



char *conf = "./server.conf";
struct User *client;

bool check_online(char *name) {
    for (int i = 0; i < MAX_CLIENT; i++) {
        if (client[i].online && strcmp(name, client[i].name) == 0) {
            printf("D: %s is already online!\n", name);
            return true;
        }
    }
    return false;
}

int find_sub() {
    for (int i = 0; i < MAX_CLIENT; i++) {
        if (client[i].online == 0) return i;
    }
    return -1;
}

//线程工作函数
void *work(void *arg) {
    printf("client login!\n");
    return NULL;
}

int main() {
    int port, server_listen, fd;
    struct RecvMsg recvmsg;
    port = atoi(get_value(conf, "SERVER_PORT"));
    client = (struct User *)calloc(MAX_CLIENT, sizeof(struct User));


    if ((server_listen = socket_create(port)) < 0) {
        perror("socket_create");
        return 1;
    }

    while (1) {
        if ((fd = accept(server_listen, NULL, NULL)) < 0) {
            perror("accept");
            continue;
        }

        recvmsg = chat_recv(fd); //收信息
        if (recvmsg.retval < 0) {
            close(fd);
            continue;
        }
        
        if (check_online(recvmsg.msg.from)) {
            //拒绝连接
        } else {
            //连接
            int sub;
            sub = find_sub();
            client[sub].online = 1;
            client[sub].fd = fd;
            strcpy(client[sub].name, recvmsg.msg.from);
            pthread_create(&client[sub].tid, NULL, work, NULL);
        }


    }

    return 0;
}
