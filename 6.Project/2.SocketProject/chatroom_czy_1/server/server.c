/*************************************************************************
	> File Name: server.c
	> Author: 少年宇
	> Mail: 
	> Created Time: 2021年03月17日 星期三 14时11分16秒
 ************************************************************************/

#include <head.h>
#include <tcp_server.h>
#include <common.h>
#include <color.h>
#include <chatroom.h>

#define MAX_CLIENT 512
char *conf = "./server.conf";

struct User {
    char name[20];
    int online;
    pthread_t tid;
    int fd;
};

struct User *client;//用户数组


int find_sub() {
    for (int i = 0; i < MAX_CLIENT; i++) {
        if (!client[i].online) return i;
    }
    return -1;
}

bool check_online(char *name) {
    for (int i = 0; i < MAX_CLIENT; i++) {
        if (client[i].online && !strcmp(name, client[i].name)) {
            printf(RED_HL("[Warning]:") " %s is already online!\n", name);
            return true;
        }
    }
    return false;
}

void *work(void *arg) {
    printf("client login!\n");
    int *sub = (int *)arg;
    int client_fd = client[*sub].fd;
    struct RecvMsg rmsg;
    while (1) {
        rmsg = chat_recv(client_fd);//收消息
        if (rmsg.retval < 0) {//收消息出错
            printf(RED_HL("[Logout]: %s\n"), client[*sub].name);
            close(client_fd);
            client[*sub].online = 0;
            return NULL;
        }

        printf(BLUE("%s : %s\n"), rmsg.msg.from, rmsg.msg.message);
    }
    return NULL;
}


int main() {
    int port, server_listen, fd;
    struct RecvMsg recvmsg;
    port = atoi(get_value(conf, "SERVER_PORT"));
    client = (struct User *)calloc(MAX_CLIENT, sizeof(struct User));

    if ((server_listen = socket_create(port)) < 0) {
        perror("socket_create");
        return 2;
    }

    while (1) {
        if ((fd = accept(server_listen, NULL, NULL)) < 0) {
            perror("accept");
            continue;
        }
        
        recvmsg = chat_recv(fd);
        if (recvmsg.retval < 0) {
            close(fd);
            continue;
        }

        if (check_online(recvmsg.msg.from)) {
            //拒绝连接
        } else {
            int sub;
            sub = find_sub();
            client[sub].online = 1;
            client[sub].fd = fd;
            strcpy(client[sub].name, recvmsg.msg.from);
            pthread_create(&client[sub].tid, NULL, work, (void *)&sub);
        }

    }



    return 0;
}
