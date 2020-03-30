# 头文件及其他文件

common目录下

> 这个目录下放的是共用的功能函数及头文件

```C
//Filename :head.h

#ifndef HEAD_H
#define HEAD_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <signal.h>

//char conf_ans[50] = {0};原本放在这个文件中的，但是后来由于多个功能函数引用head.h,会产生重复定义，所以移到common.h中了！！！！
#endif
```



```c
//Filename : common.h

#ifndef _COMMON_H
#define _COMMON_H

char conf_ans[50] = {0};
char *get_value(char *path, char *key);
void make_nonblock_ioctl(int fd);
void make_block_ioctl(int fd);
void make_nonblock(int fd);
void make_block(int fd);

#endif
```



```c
//Filename : common.c

#include "head.h"
```



```c
//Filename: chatroom.h

#ifndef _CHATROOM_H
#define _CHATROOM_H

#include "head.h"

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
    memset(&temp, 0, sizeof(temp));//一开始全置为0，表示发送成功
    //一定得返回信息结构体，那么怎么判断有没有发送成功呢？
    //那就得再封装一层，将成功与否封装在那个结构体里！于是就有了 struct RecvMsg结构体
    if (recv(fd, &temp.msg, sizeof(struct Msg), 0) <= 0) {
        temp.retval = -1;
    } 
    return temp;
}

#define MAX_CLIENT 512

#endif
```



# 初步功能框架实现

初步的功能就是

server端

1. 可以accept client，并且接收client端发过来的一条信息。
2. 根据发过来的信息结构体来检查是否重名，如果不重名，就保存用户信息到一个数组中，这个数组最多`MAX_CLIENT`个用户
3. 为每个client分配一个线程，但线程函数未实现



client端

1. 可以connect上一个server
2. 可以发送一条信息告知server自己的用户名

## Server端



配置文件

```c
//server.conf
SERVER_PORT=8888
```

server.c

```C
#include "../common/head.h"
#include "../commom/common.h"
#include "../common/tcp_server.h"
#include "../common/chatroom.h"

struct User {
    char name[20];
    int online;//判断用户是否在线的flag, 其实就是用来判断client数组中这个位置有没有被分配给某个client
    pthread_t tid;
    int fd;
};

char *conf = "./server.conf";//路径名称

struct User *client;//client数组首地址

void *work(void *arg) {
    
}

int find_sub() {
    for (int i = 0; i < MAX_CLIENT; i++) {
        if (client[i].online == 0) return i;
    }
    return -1;
}

bool check_online(char *name) {//检查name这个用户名是否被用过了，如果被用过了，那就拒绝连接这个用户
    for (int i = 0; i < MAX_CLIENT; i++) {
        if (client[i].online && !strcmp(name, client[i].name)) {
            printf("D : %s is online!\n", name);
            return true;
        }
    }
    return false;
}

int main() {//为啥不需要传参呢？因为参数已经由配置文件给出！！
    int port, server_listen, fd；
    struct RecvMsg recvmsg;//用来接收client发过来的信息
    port = atoi(get_value(conf, "SERVER_PORT"));//get_value函数实现在common.c中，实现的功能是，传入一个文件路径和一个字符串，返回那个文件中这个字符串后面的字符串；
    client = (struct User *)calloc(MAX_CLIENT, sizeof(struct User));//申请堆区内存空间
    
    if ((server_listen = socket_create(port)) < 0) {
        //socket_create(int port)定义在commom.c中，实现的功能是，传入一个端口号，将端口号绑定在某个socket上， 返回一个socket的fd;
        perror("socket_create");
        return 1;
    }
    
    //下面要开始连client了
    while (1) {
        if ((fd = accept(server_listen, NULL, NULL)) < 0) {
            perror("accept");
            continue;
        }
        
        //client登录后应该把其名字发给server端，server端接收信息并打印
        recvmsg = chat_recv(fd);//chat_recv(int fd)定义在../common/chatroom.h中，其功能是，传入一个fd，得到这个fd中发送过来的信息(RecvMsg结构体形式)
        if (recvmsg.retval < 0) {
            //如果发送出错
            close(fd);
            continue;
        }
        //但是这块有个bug，就是如果用户上线后没法送信息，就会卡在这！！
        
        //如果成功收到client端登录后发送的信息，那首先得检查是否重名，如果重名，那就拒绝连接,如果不重名，那就在client数组中找一个可用的最小下标，然后分配给这个client一个client[sub];
        if (check_online(recvmsg.msg.from)) {
            //拒接连接
        } else {
            //找最小下标并进行一系列操作
            //一系列操作其实目的就是把客户保存起来并且为他分配线程
            int sub;
            sub = find_sub();
            client[sub].online = 1;//置为在线
            client[sub].fd = fd;//保存这个文件描述符用来通信
            strcpy(client[sub].name, recvmsg.msg.from);
            pthread_create(&client[sub].tid, NULL, work, NULL);
        }
        
        
        
    }
    
    
    printf("%d\n", port);
    return 0;
}
```





## Client端

client.conf

```C
//Filename: client.conf
SERVER_IP=47.93.11.51
SERVER_PORT=8888
MY_NAME=chenzheyu
LOG_FILE=./chat.log
```

Client.c

```c
#include "../common/chatroom.h"
#include "../common/tcp_client_h"
#include "../common/common.h"

char *conf = "./client.conf"

int main() {
    int port, sockfd;
    char ip[20] = {0};
    struct Msg msg;//发送的信息结构体
    
    //获取server端的ip和端口号
    port = atoi(get_value(conf, "SERVER_PORT"));//get_value函数实现在common.c中，实现的功能是，传入一个文件路径和一个字符串，返回那个文件中这个字符串后面的字符串；
    strcpy(ip, get_value(conf, "SERVRT_IP"));
    
    //connect
    if ((sockfd = socket_connect(ip, port)) < 0) {//socket_connect(char *ip, int port)实现在tcp_client.c中，实现的功能是传入server端的ip和端口号，然后通过connect函数来进行socket连接，返回connect的return value即fd
        perror("socket_connect");
        return 1;
    }
    
    //连上server之后就要发送一条信息，告知server端自己的名字，让server端创建一个用户
    strcpy(msg.from, get_value(conf, "MY_NAME"));
    ms.flag = 2;
    if (chat_send(msg, sockfd) < 0) {
        return 2;//发送失败则return 2
    }
    
    return 0;
}
```

