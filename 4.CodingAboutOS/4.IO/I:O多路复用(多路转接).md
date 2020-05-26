# 引言

为什么要学I/O多路复用呢？

之前我们学习多进程、多线程就是为了解决高并发问题。

但是，多进程和多线程都有其缺点！



多进程

> 1. 进程数量的限制
> 2. 进程的创建、销毁及切换代价较高
> 3. 受限于CPU核心数
>    + 如果是单核，同一时刻只可能由一个进程在跑
> 4. 进程间内存隔离
> 5. 进程间通信代价较高



多线程

> 1. 受限于CPU核心数！
>
>    ***<u>这样就会影响响应能力！</u>***



之前我们写的项目的致命缺陷

> 1. 阻塞！--导致响应能力受限(***<u>例如我们写的chatroom的accept和recv</u>***)
>    + 但是如果改成非阻塞，那么就必须得循环地去检测是否有数据来！就得花更多的时间、更多的cpu次数！
> 2. 所以就没有办法感知I/O！



所以，我们引入了`I/O多路复用(多路转接)`， 它的特征

> 1. 可以感知I/O(是否可读、可写、异常)，即I/O是否ready



==与多进程和多线程技术相比，I/O多路复用技术的最大优势是系统开销小，系统不必创建进程、线程，从而大大减小了系统的开销。==



# 关于I/O多路复用的三个函数

### 三个函数之间的总结对比

即三个函数的简单对比及各优缺点

| 函数名     | 函数原型                                                     | 功能描述                                                     | RETRUN VALUE                                            |
| ---------- | ------------------------------------------------------------ | ------------------------------------------------------------ | ------------------------------------------------------- |
| select     | int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout); | monitor multiple fds, waiting until one of more fds become "ready" for I/O operation | -1 error;  0 timeout ;         number of ready success; |
| poll       | int poll(struct pollfd *fds, nfds_t nfds, int timeout);      | performs a similar task to select                            | 基本同上                                                |
| epoll(API) |                                                              | performs a similar task to poll                              |                                                         |
|            | int epoll_create(int size);                                  |                                                              | -1 error;  fd success(返回一个引用新epoll实例的fd)      |
|            | int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event) | performs control operations on the epoll instance            | -1 error; 0 success                                     |
|            | int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout) | waits for events on the epoll instance                       | 基本同poll                                              |
|            |                                                              |                                                              |                                                         |

相同点

> 1. 都是用来感知I/O操作的，功能基本相同



不同点

> ***<u>这三个函数是不断优化的</u>***！
>
> 1. ***<u>从传参角度</u>***
>
>    + select要传入三个`fd_set`的指针，这样即使你成功返回，你还得循环三个数组去找哪些是ready的
>    + poll只需要传入一个结构体指针，这样如果成功返回，你就只需要循环那一个数组就行
>    + epoll_create只传入一个size(实际上被忽略了)， 就可返回一个引用新epoll实例的fd，之后想要关注某个fd只需调用epoll_ctl就可
>
>    
>
> 2. *<u>**select的几大缺点(poll也基本差不多)**</u>*
>
>    + 每次调用，都需要把fd集合从用户态拷贝到内核态，这个开销在fd很多时会很大
>    + 同时每次调用时都需要在内核遍历传递进来的所有fd，这个开销在fd很多时也很大
>    + select支持的fd数量太小了，默认是1024
>
>    
>
> 3. ***<u>epoll对select和poll的改进</u>***
>
>    + 首先select和poll都只提供了一个函数，而epoll则是提供了3个函数
>      + `epoll_create`是创建一个引用新epoll实例的fd
>      + `epoll_ctl`是注册要监听的事件类型
>      + `epoll_wait`则是等待事件的产生
>    + 对于第一个缺点，`epoll`的解决方案是在`epoll_ctl`函数中。每次注册新的事件到epfd中时，会把所有的fd拷贝进内核，而不是在`epoll_wait`的时候重复拷贝
>    + 对于第二个缺点，epoll的解决方案不想select或poll一样每次都把current(当前进程)轮流加入fd对应的设备等待队列中，而只在epoll_ctl时把current挂一遍(这一遍必不可少)并为每个fd指定一个回调函数，当设备就绪，唤醒等待队列上的等待者时，就会调用这个回调函数，而这个回调函数会把就绪的fd加入一个就绪链表。epoll_wait的工作实际上就是在这个就绪链表中查看有没有就绪的fd
>    + 对于第三个缺点，epoll没有这个限制，它所支持的fd上限是最大可以打开的文件的数目，这个数字一般远大于2048，比如，在1GB内存的机器上大约是10W左右，具体数目可以`cat /proc/sys/fs/file-max`查看，一般来说这个数目和系统内存关系很大。
>
> 参考资料：www.cnblogs.com/Anker/p/3265058.html















# Select





## 3.使用select+自定义buffer实现echo服务器

基本要求

1. 在用户==敲两个回车==之前，消息都缓存在缓冲区里
2. 在发送过程中，可以一次发不完，但最终要发完



缓冲区：

1. 和操作系统课(heu)里学的”生产者消费者问题“里的缓冲区很像！
2. 有发送到哪个位置的pointer，也有写到哪个位置的pointer



```c
#include "../common/tcp_server.h"
#include "../common/head.h"
#include "../common/common.h"
#include "../common/color.h"

#define CLIENTSIZE 50
#define BUFSIZE 4096	

struct Buffer {/*将每个用户fd和buff数组定义在一个结构体中*/
    int fd;
    char buff[BUFSIZE];//真正的信息
    int flag;//用来标识是否收到两个回车
    int sendindex;//上次发给别人发到什么地方
    int recvindex;//写到哪个位置
};

struct Buffer *AllocBuffer() {/*init操作*/
    struct Buffer *buffer = (struct Buffer *)malloc(sizeof(struct Buffer));
    if (buffer == NULL) {
        return NULL;
    }
    buffer->fd = -1;
    buffer->flag  = buffer->recvindex = buffer->sendindex = 0;
    memset(buffer->buff, 0, BUFSIZE);
    return buffer;
}

void FreeBuffer(struct Buffer *buffer) {/*free操作*/
    free(buffer);
}

int RecvToBuff(int fd, struct Buffer *buffer) {/*从fd中收信息,经大小写转化后放到buffer中*/
	char buff[BUFSIZE] = {0};/*必须清空*/
    int recv_num = 0;
    while (1) {
        recv_num = recv(fd, buff, sizeof(buff), 0);/*从fd中收信息放到buff中*/
        if (recv_num <= 0) break;/*出错了，又由于是非阻塞IO，就得break，看到底出啥错了*/
        for (int i = 0; i < recv_num; i++) {
            if (buffer->recvindex < sizeof(buffer-buff)) {
                /*可以继续往后写*/
                buffer->buff[buffer->recvindex++] = ch_char(buff[i]);
            }
            if (buffer->recvindex > 1 && buffer->buff[buffer->recvindex - 1] == '\n' && buffer->buff[buffer->recvindex - 2] == '\n') {
                /*收到两个回车，就可以发回去了*/
                buffer->flag = 1;
            }
        }
    }
    
    /*判断出错的情况*/
    if (recv_num < 0) {
        if (errno == EAGAIN) /*表明不是严重出错*/
            return 0;
        return -1;/*真出错了*/
    } else if (recv_num == 0) {/*表示收无可收了，应该关闭连接了*/
        return 1;
    }
}

int SendFromBuffer(int fd, struct Buffer *buffer) {/*从buffer里读出来发到fd中*/
    int send_num = 0;
    while (buffer->sendindex < buffer->recvindex) {/*意味着有真是数据*/
        send_num = send(fd, buffer->buff + buffer->sendindex, buffer->recvindex - buffer->sendindex, 0);/*发送数据*/
        if (send_num < 0) {
            if (errno == EAGAIN) /*不是严重错误*/
                return 0;
            /*如果进行到这一行，表明连接已经失效*/
            buffer->fd = -1;
            return -1;
        }
        buffer->sendindex += send_num;
    }
    if (buffer->sendindex == buffer->recvindex) {/*表明数据全发完了*/
        buffer->sendindex = buffer->recvindex = 0;
    }
    buffer->flag = 0;
    return 0;/*0表示发送成功*/
}


char ch_char(char c) {/*把小写字母转化为大写字母*/
    if (c >= 'a' && c <= 'z') return c-32;
    return c;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage : %s port!\n", argv[0]);
        exit(1);
    }
    
    int server_listen, fd, max_fd;
    if ((server_listen = socket_create(atoi(argv[1]))) < 0) {
        /*socker_create(int port)是定义在"../common/tcp_server.c中的一个函数，其功能是创建一个监听状态的socket"*/
        perror("socker_create");
        exit(1;)
    }
    
    /*定义用户缓存空间*/
    struct Buffer *buffer[CLIENTSIZE];/*数组的每一位都是一个指针变量*/
    
    for (int i = 0; i < CLIENTSIZE; i++) {/*申请数组中的每个空间*/
        buffer[i] = AllocBuffer();
    }
    
    make_nonblock(server_listen);/*置为非阻塞套接字，该函数定义在"../common/common.c中"*/
    
    fd_set rfds, wfds, efds;
    max_fd = server_listen;/*设置server_listen为最大的fd*/
    
    while (1) {
        FD_ZERO(&rfds);
        FD_ZERO(&wfds);
        FD_ZERO(&efds);
        
        FD_SET(server_listen, &rfds);
        for (int i = 0; i < CLIENTSIZE; i++) {/*遍历数组，如果有真实连接就放入监听集合*/
            if (buffer[i]->fd == server_listen) continue;
            if (buffer[i]->fd > 0) {/*表明有真实的连接*/
                if (max_fd < buffer[i]->fd) max_fd = buffer[i]->fd;
                FD_SET(buffer[i]->fd, &rfds);
                if (buffer[i]->flag == 1) {/*表明这个连接可以写回去*/
                    FD_SET(buffer[i]->fd, &wfds);
                }
            }
        }
        
        /*判断监听结果*/
        if (select(max_fd + 1, &rfds, &wfds, NULL, NULL) < 0) {
            perror("select");
            return 1;
        }
        
        if (FD_ISSET(server_listen, &rfds)) {
            printf("Connect ready on serverlisten!\n");
            if ((fd = accept(server_listen, NULL, NULL)) < 0) {
                perror("accept");
                return 1;
            }
            if (fd > CLIENTSIZE) {
                printf("Too many clients!\n");
                close(fd);
            } else {
                printf("Login!\n")
                make_nonblock(fd);/*设置为非阻塞*/
                if (buffer[fd]->fd == -1) 
                    buffer[fd]->fd = fd;/*这里我们给的语义信息是，数组下标就是其fd！*/
            }
        }
        
        /*接下来开始处理收发信息*/
        for (int i = 0; i <= max_fd; i++) {/*一定要<=而不是<！！！！*/
            int retval = 0;/*一定要初始化为0！！否则就会一登录就退出！*/
            if (i == server_listen) continue;
            if (FD_ISSET(i, &rfds)) {/*如果可读*/
                retval = RecvToBuffer(i, buffer[i]);/*收信息*/
            }
            if (retval == 0 && FD_ISSET(i, &wfds)) {
                /*如果收无可收了(感觉此处有bug)并且此时也可写*/
                retval = SendFromBuffer(i, buffer[i]);/*发信息*/
            }
            if (retval) {/*非0表示出错了或者关闭了*/
                printf("Logout!\n");
                buffer[i]->fd = -1;
                close(i);
            }
        }
        
    }
    return 0;
}
```



