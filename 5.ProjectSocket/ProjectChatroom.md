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



# 1.初步功能框架实现

**<u>*初步的功能就是可以连接上了，但是还没能实现具体收发信息</u>***

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
    printf("client login!\n");
    return NULL;
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
            printf("D : %s is already online!\n", name);
            return true;
        }
    }
    return false;
}

int main() {//为啥不需要传参呢？因为参数已经由配置文件给出！！
    int port, server_listen, fd；
    struct RecvMsg recvmsg;//用来接收client发过来的信息
    struct Msg msg;//用来发送信息给client
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







# 2.进一步功能实现

即使用多线程、多进程初步实现聊天功能

server端：

1. 线程处理函数work的实现
2. 



client端：

1. 接收server端对登录操作的反馈信息及后续操作
2. 在子进程中发送消息





## Server端

增加的功能

1. 原17~19行的work函数
2. 拒绝连接的处理方法





```c
void *work(void *arg) {
    int *sub = (int *)arg;
    int client_fd = client[*sub].fd;
    
    struct RecvMsg rmsg;
    printf(GREEN"Login "NONE": %s\n", client[*sub].name);
    
    //接下来要收信息了
    struct RecvMsg rmsg;
    while (1) {
        rmsg = chat_recv(client_fd);
        if (rmsg.retval < 0) {//如果接收失败
            printf(PINK"Logout:"NONE" %s \n", client[*sub].name);
            close(client_fd);
            client[*sub].online = 0;
            return NULL;
        }
        //如果成功接收信息
        printf(BLUE"%s"NONE" :%s\n", rmsg.msg.from, rmsg.msg.message);
    }
    return NULL;
}

//原81行
pthread_create(&client[sub].tid, NULL, work, (void *)&sub);
```

```c
//原67行的拒接连接处理方法

if (check_online(recv.msg.from)) {
    msg.flag = 3;
    strcpy(msg.messag, "You have Already Login System!\n");
    chat_send(msg, fd);//发送信息给client表示拒接连接
    close(fd);
    continue;
}

//原72行开始，接收连接的处理方法
msg.flag = 2;
strcpy(msg.message, "Welcome to this chatroom!\n");
chat_send(msg, fd);
```



## Client端

1. 接收server端对登录操作的反馈信息及后续操作

```c
#include "../common/color.h"

//从原来的29行开始添加

//因为我必须知道server端是拒接我还是连接我，所以得从server端接收信息
struct RecvMsg rmsg = chat_recv(sockfd);
if (rmsg.retval < 0) {//如果接收失败
    fprintf(stderr, "Error!\n");
    return -1;
}
//打印server端发来的信息
printf(GREEN"Server"NONE": %s\n", rmsg.msg.message);

if (rmsg.msg.flag == 3) {//flag==3表示我重名了，server端要求我断开连接
    close(sock_fd);
    return 1;
}
```



2.如果server端允许我连接之后,创建子进程用于发送信息

```c
//接上面的17行后面写，就是如果server端允许我连接之后,创建子进程用于发送信息

pid_t pid;

if ((pid = fork()) < 0) {
    perror("fork");
}

//子进程
if (pid = 0) {
    system("clear");//发信息之前清空屏幕
    while (1) {
        printf(L_PINK"Please Input Message:"NONE"\n");
        scanf("%[^\n]s, msg.message");
        getchar();
        chat_send(msg, sockfd);//发送信息
        memset(msg.message, 0, sizeof(msg.message));
        system("clear");//发送信息之后也清空屏幕
    }
} else {
    //父进程
    wait(NULL);
}
```



# 出现的bug

1. 退出的时候，显示的是上一个登录进来的用户的名字

   + 原因：在线程函数`work()`中，sub指向的地址会随其他线程的进行而改变！！这是多线程必然带来的问题！！

   + 解决方法：work()函数中，不定义指针变量，直接保存下标！即将`int *sub = (int *)arg`改成`int sub = *(int *)arg`；







2. 在client端运行时，ctrl + c会退出程序，但是不会退出用户！

   原因：

   1. `ctrl + c`其实是一个信号
   2. 用`kill -l`可以列出linux中的所有信号

   ![img](https://wx3.sinaimg.cn/mw690/005LasY6gy1gdd7ijooi7j311a0f21al.jpg)

   3. ==系统通过信号告诉进程它应该干什么事情==

   4. `ctrl + c`就是信号2)，就是中断信号

   解决办法：

   1. 用`signal()`函数

      ![img](https://wx3.sinaimg.cn/mw690/005LasY6gy1gdd89lqwqpj31cd0u0hch.jpg)

   2. 由description可知，这个函数设置了disposition of 第一个参数，具体设置方式就是第二个参数handler，它可以是`SIG_IGN, SIG_DFL`或者是自己定义的一个函数

   3. 所以代码如下

      在client.c中

      ```c
      //在“进一步功能实现的”client.c中的11行之前加上
      signal(SIGINT, logout);
      
      //logout的函数声明与定义
      void logout(int signalnum) {
          close(sockfd);//注意还得将sockfd设置为全局变量
          printf("您已退出！\n");
          exit(1);
          return;
      }
      
      //接着在22行下面加上
      close(sockfd);//为什么父进程中也要close呢？因为子进程是完全复制了父进程的内容，所以子进程关掉的sockfd是子进程中的，父进程中的sockfd没关掉
      
      ```

      

3. client端运行时，如果按下`Ctrl + D(即EOF)`，也结束不了进程

   解决办法：

   ```c
   //在“进一步功能实现”的client.c中的子进程中
   //在while外加上
   char c = 'a';
   
   //while内
   while (c != EOF) {
       //scanf下面一行改成
       c = getchar();
   }
   
//复习的时候发现这样做并不能解决这个bug！！！！
   ```
   
   





# 3.增加的功能1

即将公聊信息转发给所有人



## Server端

1. 很显然，server端需要在线程函数`work()`中做文章



```c
//Filename: server.c

//在“进一步功能实现”的work函数的19行下面加上
if (rmsg.msg.flag == 0) {
    send_all(rmsg.msg);
} else {
    printf("这是一条私聊信息\n");
}

//send_all函数的实现
void send_all(struct Msg msg) {
    for (int i = 0; i < MAX_CLIENT; i++) {
        if (!clien[i].online) continue;
        chat_send(msg, client[i].fd);
    }
}
```





## Client端

1. 原来的子进程用于发送信息，那么父进程就可以用来接收公聊信息！



```c
//Filename : client.c

//在父进程的wait(NULL)前面加上
while (1) {
    FILE *log_fp = fopen(logfile, "w");
    struct RecvMsg rmsg;
    if (rmsg.msg.flag == 0) {
    	fprintf(log_fp, L_BLUE"%s : "NONE"%s\n", rmsg.msg.from, rmsg.msg.message);
        printf(L_BLUE"%s : "NONE"%s\n", rmsg.msg.from, rmsg.msg.message);
        fflush(log_fp);//这一行一定要！！！！！！！
    }
}


//在主函数前定义一个logfile字符串，用来存日志文件的文件名
char logfile[50] = {0};


//在主函数一开始加上
strcpy(logfile, get_value(conf, "LOG_FILE"));

//如果想试验的话就在子进程(用来发送信息)中加上
msg.flag = 0;//标志这是个公聊信息！
```

PS：除了fprintf之外，也可以用`freopen(log_file, "a+", stdout);`



BUG：

1. 自己发的公聊信息，自己收不到！！

   原因：`system("clear")`





# 4.增加的功能2

即对私聊信息的处理





## Server端



```c
//Filename : server.c

//在“增加的功能1”中的server端的代码第6行
//把else及其后面的一个语句改成

else if (rmsg.msg.flag == 1) {
    if (rmsg.msg.message[0] == '@') {
        char to[20] = {0};
        //拷贝名字
        int i = 0;
        for (i = 1; i < 20; i++) {
            if (rmsg.msg.message[i] ==' ')
                break;
        }
        strncpy(to, rmsg.msg.message + 1, i - 1);
        //检查用户是否存在
        
        int ind;
        if ((ind = check_name(to)) < 0) {
            //告知不在线
            sprintf(rmsg.msg.message, "%s is not online!\n", to);
            chat_send(rmsg.msg, client_fd);
            continue;
        }
        //如果用户在线
        rmsg.msg.flag = 1;//标志为私聊信息
        chat_send(rmsg.msg, client[ind].fd);
    }
}


//check_name()的实现
int check_name(char *name) {
    for (int i = 0; i < MAX_CLIENT; i++) {
        if (client[i].online && !strcmp(client[i].name, name))
            return i;
    }
    return -1;
}

```



## Client端



```c
//Filename : client.c

//在“出现的bug”中client.c的`c = getchar();`//下面加上
if (msg.message[0] == '@') {
    msg.flag = 1;
}

//在“增加的功能1”中的client.c的第九行加上
  else if (rmsg.msg.flag == 2) {
   	printf(YELLOW"通知信息: "NONE"%s\n", rmsg.msg.message);
} else if (rmsg.msg.flag == 1) {
      printf(L_BULE"%s"L_GREEN"*"NONE": %s\n", rmsg.msg.from, rmsg.msg.message);
} else {
      printf("Error!\n");
  }
	fflush(stdout);//不懂为啥要加这句？？
```









# 2刷有些没弄懂的地方

## 1.common

1. get_value()里的一些api比如getline什么的