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


void send_all(struct Msg msg) {
    for (int i = 0; i < MAX_CLIENT; i++) {
        if (!client[i].online) continue;
        chat_send(msg, client[i].fd);
    }
    return ;
}

int check_name(char * name) {
    for (int i = 0; i < MAX_CLIENT; i++) {
        if (client[i].online && !strcmp(client[i].name, name)) return i;
    }
    return -1;
}


void *work(void *arg) {
    int sub = *(int *)arg;
    int client_fd = client[sub].fd;
    struct RecvMsg rmsg;
    printf(GREEN_HL("[login]: ") "%s\n", client[sub].name);

    while (1) {
        rmsg = chat_recv(client_fd);//收消息
        if (rmsg.retval < 0) {//收消息出错表明对方已下线
            printf(RED_HL("[Logout]: ") "%s\n", client[sub].name);
            close(client_fd);
            client[sub].online = 0;
            return NULL;
        }

        //在server的stdout上打印
        printf(BLUE("%s : ") "%s\n", rmsg.msg.from, rmsg.msg.message);

        //判断信息类型之后看要不要转发给每个client
        if (rmsg.msg.flag == 0) {//公聊
            send_all(rmsg.msg);
        } else if (rmsg.msg.flag == 1) {//私聊
            if (rmsg.msg.message[0] != '@') continue;//其实感觉没必要再判断？
            char to[20] = {0};
            int i = 1;
            for (; i <= 20; i++) {
                if (rmsg.msg.message[i] == ' ') break;
            }
            strncpy(to, rmsg.msg.message + 1, i - 1);

            int ind;
            if ((ind = check_name(to)) < 0) {
                //告知不在线
                sprintf(rmsg.msg.message, "%s is not online", to);
                rmsg.msg.flag = 2;
                chat_send(rmsg.msg, client_fd);
                continue;
            }
            rmsg.msg.flag = 1;
            chat_send(rmsg.msg, client[ind].fd);
        }

    }
    return NULL;
}


int main() {
    int port, server_listen, fd;
    struct RecvMsg recvmsg;
    struct Msg msg;
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
            msg.flag = 3;
            strcpy(msg.message, "You are already online!");
            chat_send(msg, fd);
            close(fd);
        } else {
            //接受连接
            msg.flag = 2;
            strcpy(msg.message, "Welcome to this chatroom!");
            chat_send(msg, fd);

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
