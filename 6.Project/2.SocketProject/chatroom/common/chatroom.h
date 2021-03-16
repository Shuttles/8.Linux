/*************************************************************************
	> File Name: chatroom.h
	> Author: 少年宇
	> Mail: 
	> Created Time: 2021年01月16日 星期六 20时48分34秒
 ************************************************************************/

#ifndef _CHATROOM_H
#define _CHATROOM_H

#include "head.h"

#define MAX_CLIENT 512
struct Msg {
    char from[20];
    int flag;
    char message[512];
};

struct RecvMsg {
    struct Msg msg;
    int retval;
};

int chat_send(struct Msg msg, int fd) {
    if (send(fd, (void *)&msg, sizeof(msg), 0) <= 0) {
        return -1;
    }
    return 0;
}

struct RecvMsg chat_recv(int fd) {
    struct RecvMsg temp;
    memset(&temp, 0, sizeof(temp));
    if (recv(fd, &temp.msg, sizeof(struct Msg), 0) <= 0) {
        temp.retval = -1;
    } 
    return temp;
}

#endif
