# 1.为什么要写这个软件？

1. 理解整包、拆包、粘包
2. 理解拆包和粘包的产生原因，掌握处理方法
3. 进一步了解底层协议乃至所有网络编程中对数据流的处理



## 原因

1. 如果你发送的文件较大，那么底层TCP协议都是分成一块一块来发出去的。
2. 所以通过fread等读出来的数据不一定就能够占满一个固定大小



## 拆包

假设你的一个报文被分成5个TCP包，那么可能，当前3个包到达对端之后就会提交给对端应用层，(即使在应用层看来报文还不是完整的)，因为这3个包已经处于就绪状态！

所以我们只能在应用层编程的时候处理这个问题！



## 粘包

如果上一个报文出现拆包，(比如说后俩个TCP包没提交给应用层)，那么下一次提交给应用层一定会出现粘包(上次没提交的后2个和这次要提交的几个包构成粘包)！





## 处理方法

1. 准备三个报文变量

   ![img](https://wx4.sinaimg.cn/mw690/005LasY6gy1gdgono6upbj31no0r47dr.jpg)

   + 每次接受数据的时候先存放在`packet_t`中，检查是否为整包
   + `packet`用来存放真正的数据
   + `packet_pre`用来存放出现粘包后的切分拆包
   + `offset`表示偏移量，`==0`表示上次收到的是个整包

2. ![img](https://wx4.sinaimg.cn/mw690/005LasY6gy1gdgosahbplj31jv0u0dss.jpg)

   + 这种情况表明，收到的是个整包。

   + 那就复制到`packet`里，并提交给应用层。

   + 此时`offset`还是置为0

3. ![img](https://wx1.sinaimg.cn/mw690/005LasY6gy1gdgouqqg50j31jh0u04b2.jpg)

   + 这种情况，表明收到的是个拆包。

   + 那也复制到`packet`里，但是不提交给应用层。

   + 此时`offset`应置为这个粘包的大小。

4. ![img](https://wx1.sinaimg.cn/mw690/005LasY6gy1gdgox4j2h4j31ck0u0qe6.jpg)

   + 如果上次是个拆包，那么这次一定收到的是个粘包
   + 这意味着红色那部分里有几个包是和后面的绿色部分一起的，它们将构成一个整包
   + 处理办法就是进行切分，切出前一部分，复制到`packet`中，并提交给应用层

   ![img](https://wx3.sinaimg.cn/mw690/005LasY6gy1gdgp05d2qpj31e80u0gz4.jpg)

   ![img](https://wx3.sinaimg.cn/mw690/005LasY6gy1gdgp2bq9zfj31aq0u07ha.jpg)

   + 接着把`packet_t`中的剩下的部分复制到`packet_pre`中
   + 并设置`offset`

   ![img](https://wx4.sinaimg.cn/mw690/005LasY6gy1gdgp3em5wlj31cz0u0alt.jpg)

   + 接着还得处理`packet_pre`，将其复制到`packet`中
   + `offset`不变

   ![img](https://wx3.sinaimg.cn/mw690/005LasY6gy1gdgp51qyg2j31cc0u07ej.jpg)





仔细想想，`packet_pre`根本不需要！！















# 实现

> 路径
>
> `SocketProject/filetransfer`



## Sender.c

编译命令

```shell
gcc sender.c ./common/common.c ./common/tcp_client.c -o client
```

其实就相当于`Client`端

```c
#include "./common/head.h"
#include "./common/common.h"
#include "./common/tcp_client.h"

struct FileMsg {
    long size;//文件大小，用来检验收到的文件大小与原文件大小是否一致
   	char name[50];//文件名
    char buf[4096];//文件真实信息
};

int main(int argc, char **argv) {	
    if (argc != 3) {
        fprintf(stderr, "Usage : %s ip port!\n", argv[0]);
        return 1;
    }
    
    int sockfd, port;
    char buff[100] = {0};//存放指令
    char name[50] = {0};//文件名
    struct FileMsg filemsg;
    
    port = atoi(argv[2]);
    if ((sockfd = socket_connect(argv[1], port)) < 0) {
        //socket_connect(char *ip, int port)实现在tcp_client.h中，实现的功能是完成socket连接
        perror("socket_connect");
        return 1;
    }
    
    //连接上之后
    while (1) {
        scanf("%[^\n]s", buff);
        getchar();//一定要吞回车！！！！！！！！！！否则后果很严重！！
        if (!strncmp("send ", buff, 5)) {
            //表示这是我们能认识的一个正确指令
            strcpy(name, buff + 5);
        } else {
            fprintf(stderr, "invalid command!\n");
            continue;
        }
        
        //打开文件
        FILE *fp = NULL;
        size_t size;//记录每次读了多少
        if ((fp = fopen(name, "rb")) == NULL) {
            //发送文件的时候要以二进制形式打开！
            perror("fopen");
            continue;
        } 
        
        //打开文件后我们要知道文件的大小
        //通过fseek()
        //fseek就是在文件流中移动，可以把文件指针移到任何位置！
        //如果文件指针移动到了文件末尾，那就知道了文件的大小！！
        //更详细的关于fseek的资料请往下翻！
        fseek(fp, 0L, SEEK_END);//L表示长整型
        filemsg.size = ftell(fp);
        strcpy(filemsg.name, name);
        
        fseek(fp, 0L, SEEK_SET);//将文件指针重新指向开头
        
        //接下来就开始读文件了！
        //用fread()，因为它是`binary stream input/output`
        while ((size = fread(filemsg.buf, 1, 4096, fp))) {
            //收完一次就发
            send(sockfd, (void *)&filemsg, sizeof(filemsg), 0);
            //send 还不太熟悉！！！！！
            memset(filemsg.buf, 0, sizeof(filemsg.buf));
        }
        printf("After Send!\n");
    }
    close(sockfd);
    fclose(fp);
    return 0;
}
```



### fseek()

![img](https://wx4.sinaimg.cn/mw690/005LasY6gy1gdgxssyv4hj31560u04qp.jpg)

![img](https://wx2.sinaimg.cn/mw690/005LasY6gy1gdgxutoeb1j318d0u0kds.jpg)

![img](https://wx4.sinaimg.cn/mw690/005LasY6gy1gdgxxpzsaoj31bc078dl9.jpg)

1. 由description可知，`fseek()`可以重置文件指针的位置，第三个参数是开始移动的起点，第二个参数offset是移动的字节数(从起点往后移动)

2. 由returnvalue可知，如果成功，有几个函数返回 0，但是`ftell()`返回当前的偏移量！

   ***<u>由此，我们可以先将文件指针移到文件末尾，再调用`ftell()`就可以获得文件长度了！</u>***



### fread()

![img](https://wx1.sinaimg.cn/mw690/005LasY6gy1gdgy4k3hddj318h0u04ja.jpg)

1. 由description可知，。。。第一段写得很明白啦





## recver.c

1. 就相当于`server.c`
2. 要在本地打开一个文件往里写收到的内容
3. 收的同时要判断这个文件有没有收全！(通过文件大小)



编译命令

```shell
gcc recver.c ./common/common.c ./common/tcp_server.c -o server
```



```c
#include "./common/head.h"
#include "./common/tcp_server.c"
#include "./common/common.h"

struct FileMsg {
    long size;//文件大小，用来检验收到的文件大小与原文件大小是否一致
   	char name[50];//文件名
    char buf[4096];//文件真实信息
};

void signal_handler(int sig) {
    wait(NULL);
}


// 这是原来的不考虑拆包、粘包问题时写的child_do！！
void child_do(int fd) {
    size_t recv_size;
    struct FileMsg packet_t;
    int packet_size = sizeof(struct FileMsg);
    printf("Before recv!\n");
    while ((recv_size = recv(fd, (void *)&packet_t, packet_size))) {
        printf("recv_size = %d\n", recv_size);
    }
}


//下面是考虑了这些问题后写的！！
void child_do(int fd) {
    size_t recv_size, size = 0;
    struct FileMsg packet_t, packet, packet_pre;
    int packet_size = sizeof(struct FileMsg);
    int offset, flag;//flag定义为packet_pre里的offset！!
    long filesize;
    int cnt = 0;//判断是否为第一个包！！
    printf("Before recv!\n");
    FILE *fp = NULL;
    while (1) {
        //应该是个死循环，收n次整包！！！
        if (flag) {
            //意味着上次有粘包，拆包后在packer_pre里！！
            memcpy(&packet, &packer_pre, flag);
            offset = flag;//offset应等于上次剩下的大小！！
        }
        flag = 0;
        while ((recv_size = recv(fd, (void *)&packet_t, packet_size, 0) > 0) {
            //这个循环是为了保证每次都能收一个整包！！
            if (offset + recv_size == packet_size) {
                //如果是个整包！
                memcpy((char *)&packet + offset, &packet_t, recv_size);
                offset = 0;
                printf("整包 size = %d!\n", offset + recv_size);
                break;
            } else if (offset + recv_size < packet_size) {
                //拆包！
               	memcpy((char *)&packet + offset, &packet_t, recv_size); 
               	offset += recv_size;
                printf("拆包 size = %d!\n", offset);
            } else {
                //粘包！！
                memcpy((char *)&packet + offset, &packet_t, packet_size - offset);
                flag = recv_size - (packet_size - offset);
                memcpy(&packet_pre, (char *)&packet_t + packet_size - offset, flag);
                offset = 0;
                printf("粘包 size = %d!\n", flag);
                break;//只有整包和拆包要break！！！
            }
        }
        if (!cnt) {
            //如果是收到的第一个包！那就找文件名！！
            printf("recv %s with size = %d\n", packet.name, packet.size);
            if ((fp = fopen(packet.name), "wb") == NULL) {
                perror("fopen");
                break;
            }
            
        }
        cnt++;
       	
       //要在本地开始写文件了
       size_t write_size = 0;
       if (packet.size - size >= packet_size) {
           write_size = fwrite(packet.buf, 1, sizeof(packet.buf), fp);
       } else {
           //不是一个整包
           write_size = fwrite(packet.buf, 1, packet.size - size, fp);
       }
       size += write_size;
               
       if (size >= packet.size) {
           printf("Finished : %s!\n", packet.name);
           break;
       }
    }
    close(fd);
    fclose(fp);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage : %s port!\n", argv[0]);
        return 1;
    }
    
    int server_listen, port, fd;
    pid_t pid;
    
    port = atoi(argv[1]);
    
    if ((server_listen = socket_create(port)) < 0) {
        //socket_create(int port)实现在tcp_server.c中，实现的功能是创建一个监听状态的socket！！
        perror("socket_create");
        return 1;
    }
    
    signal(SIGCHLD, signal_handler);//在此注册个signal函数！
    
    //该收文件了！
    while (1) {
        if ((fd = accept(server_listen, NULL, NULL)) < 0) {
            perror("accept");
            continue;
        }
        
        printf("After accept!\n");
        
        //一般来说，都是子进程用来做事情，父进程用来循环等待去重新获取连接！
        if ((pid = fork()) < 0) {
            perror("fork");
            continue;
        }
        if (pid == 0) {
            close(server_listen);//因为子进程一定用不上server_listen了
            child_do(fd);
            exit(0);//一定要exit！！！！否则后果你懂的！！
        } else {
            close(fd);//父进程一定用不上fd了
        }
        
        
    }
    
    return 0;
}
```

