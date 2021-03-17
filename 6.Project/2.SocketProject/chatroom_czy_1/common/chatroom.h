/*************************************************************************
	> File Name: chatroom.h
	> Author: 少年宇
	> Mail: 
	> Created Time: 2021年03月17日 星期三 16时51分12秒
 ************************************************************************/

#ifndef _CHATROOM_H
#define _CHATROOM_H

#include <head.h>

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
    memset(&temp, 0, sizeof(temp));//0代表没出错
    if (recv(fd, &temp.msg, sizeof(struct Msg), 0) <= 0) {
        temp.retval = -1;
    } 
    return temp;
}
#endif
