# 为什么要用socket编程？

![img](https://wx2.sinaimg.cn/mw690/005LasY6gy1gd41c3naqoj31dk0u01b5.jpg)

![img](https://wx4.sinaimg.cn/mw690/005LasY6gy1gd41d0nj13j31l60u01f4.jpg)

进程不是凭空出现的！！是通过clone得来的！！

系统(比喻中的有ip地址的大楼)起来之后，一开始里面是没有多少房子(进程)的，后来因为我们起的进程越来越多，房子也就越来越多，每个房子都可能复制出更多房子。

但是复制出新的房子之后，它们是没有门的，也就没法进行网络通讯，如果想有门，你得告诉系统我现在寂寞了，我想跟我对象通信，你得给我分个门，门牌号即为端口。

如何向系统要门？

通过socket函数！！



PS:你可以有多个对象(纯属比喻)，所以你的进程可以有多个门，用来和每个对象通信！



# 创建socket

![img](https://wx1.sinaimg.cn/mw690/005LasY6gy1gd4218nfwvj31cq0u0h8g.jpg)

1. ***<u>就是通过socket函数来创建socket即为一个房间(进程)装一扇门(暂时还没有门牌号即端口)</u>***。

2. 第一个参数domain就是ipv4/6、本地网络的还是路由网路的。

   通常都用ipv4即AF_INET。

3. 第二个参数type是类型

   + `SOCK_STREAM`是指希望通信时数据是流式传递的。
   + `SOCK_DGRAM`是指希望通信时数据是以用户数据报文传递(只有UDP方式时才可用)
   + 后俩暂时用不到。

4. 第三个参数就是协议

   + 如果第二个参数选`SOCK_STREAM`，那这个参数必须选TCP。
   + 如果第二个参数选`SOCK_DGRAM`，那这个参数必须选UDP
   + 如果前俩已经确定第三个参数必须选啥了，那么这个参数可以写为0





# 如何要一个门牌号(端口)

![img](https://wx2.sinaimg.cn/mw690/005LasY6gy1gd43uzzckaj31lm0paauk.jpg)

1. 通过`BIND`函数来自己设定端口

2. 第一个参数是`socket`返回的文件描述符。

   为什么要有这个参数呢？

   因为你可能有多个对象，要和她们每个通信，就得有很多门，所以就有很多个`socket`函数被调用。





## 大小端

![img](https://wx4.sinaimg.cn/mw690/005LasY6gy1gd44x48zrpj31pm0u0kjl.jpg)

小端

![img](https://wx2.sinaimg.cn/mw690/005LasY6gy1gd44ywnxo3j31820u0npd.jpg)



## 相关结构体

![img](https://wx2.sinaimg.cn/mw690/005LasY6gy1gd442it3amj311u0u07ky.jpg)

1. 地址族就是是ipv4/6/还是本地/路由

2. 14字节那个就是放的ip地址和端口信息

   + 端口是自己的端口(门牌号)
   + ip地址是对方的ip地址

   + ip地址可以设为一个固定的地址，这就意味你只能接受从那个ip地址发过来的数据
   + 一般都设置为`0.0.0.0`

3. 由于14字节的不好区分ip地址和端口，所以我们通常用第二个结构体

   + `struct in_addr`中只有一个`uint32_t`，而ip地址也是`uint32_t`，所以这个结构体就是用来存ip地址的
   + 所以第二个结构体很好区分ip地址和端口信息
   + ***<u>所以我们通常都用第二个结构体作为实参(要强转)</u>***



## 要用到的函数

![img](https://wx1.sinaimg.cn/mw690/005LasY6gy1gd44fgjztej31om0u0qv5.jpg)

1. 端口号最大值是65535(因为是用`UINT16_t`来存的)

2. 前俩函数是用来处理端口号的

   + 第一个函数很好记

     h是指`host(本地主机字节序)`，to不用解释，`ns是network`网络字节序

   + 第二个函数则相反

3. 后俩函数是用来处理ip地址的

   都用在那个我们常用的结构体`struct sockaddr_in`的变量处理中。

   他俩作用相反！！

   `ntoa`中的`n`是指网络字节序，`a`是指字符串(`array`)

4. `inet_addr(INADDR_ANY)`指的是我有很多个对象，无论从哪个ip传过来的数据我都要，`INADDR_ANT`相当于`0.0.0.0`这个ip





# 被动等待通信

![img](https://wx4.sinaimg.cn/mw690/005LasY6gy1gd4521ngk7j318i0oiari.jpg)

1. `listen`函数将一个`socket`转换为被动等待的状态，这个时候只能等待别人的通信，不能主动联系别人
2. `backlog`的意思是，如果设置为10，那么当你和一个人通信时，等待与你通信的人排成的队列人数不能超过10





# 接受连接

![img](https://wx2.sinaimg.cn/mw690/005LasY6gy1gd456qtto1j31yc0rie81.jpg)

1. 第一个参数就是去哪个门接受连接

2. 第二个参数是一个传出参数，用来传出对方的地址信息

   如果你不想知道这个连接是从哪个地址过来的，你就可以把第二、三个参数都设为`NULL`。

3. 有一个问题，如果你accept了一个连接，那么如果这时别人要连你，你还能不能accept了？

   可以的。

   因为当你接收了一个连接之后，accept函数会clone出一个分身来处理这个连接，这就相当于游戏里用来引导你完成任务的`NPC`，每个玩家到这个地方都会蹦出来个`NPC`，而不是你带走了一个`NPC`之后就没人引导别的玩家了！

   



# 也可主动建立连接

![img](https://wx3.sinaimg.cn/mw690/005LasY6gy1gd45nczo3vj31t20pgha4.jpg)

1. 当你有对象时，你可以等待连接，但是当你没对象时，你就得主动去建立连接(给别人传数据)

   不用`bind、listen、accept`！！！！！

   不用`bind`意味着你连门牌号(端口)都不用！！

   因为系统会直接给你分配个端口！

2. 参数是目的地端(你想连接的人)的端口号





发送数据和接受数据的底层实现都是与`read、write`相关的！！

# 发送数据

![img](https://wx4.sinaimg.cn/mw690/005LasY6gy1gd45w366xtj31qk0re7vn.jpg)

flags通常写0

# 接收数据

![img](https://wx3.sinaimg.cn/mw690/005LasY6gy1gd45xnrl9xj31r00t4qsu.jpg)



1. 和`write很像`
2. 第二个参数是把数据接收到哪去，第三个参数是接收数据的最大长度

# 关闭连接

![img](https://wx2.sinaimg.cn/mw690/005LasY6gy1gd45v7mqyhj30xi0kc454.jpg)



# 其他

![img](https://wx1.sinaimg.cn/mw690/005LasY6gy1gd45tqqvorj31p00u04qp.jpg)



![img](https://wx2.sinaimg.cn/mw690/005LasY6gy1gd45ugh9rmj31i40n4qm7.jpg)





# 程序

## Server端

为了能让大家连接，所以server端必须写在公网上

Server端通过socket要了门之后，要通过bind要门牌号(端口)

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]) {
  if (argc != 2) {//如果不是两个参数也就意味着没有问系统要端口
    fprintf(stderr, "Usage : %s port\n", argv[0]);
    exit(1);
  }
  //定义端口
  int port, server_listen;//server_listen作为socket文件描述符
  port = atoi(argv[1]);//将传入的第二个参数转化为整形作为端口号
  
  //启动socket，即向系统申请了一扇门
  if ((server_listen = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket");
    exit(1);
  }
  
  printf("Socket_ceate.\n");//输出调试
  //要门牌号
  struct sockaddr_in server;
  server.sin_family = AF_INET;
  server.sin_port = htons(port);
  server.sin_addr.s_addr = INADDR_ANY;//这三行详情请看“相关结构体、要用到的函数”
  if (bind(server_listen, (struct sockaddr *)&server, sizeof(server)) < 0) {
    //如果出错，意味着这个端口号不能给你
    perror("bind");
    exit(1);
  }
  
  printf("Socket bind.\n");//输出调试
  //接下来要listen
  if (listen(server_listen, 20) < 0) {
    perror("server");
    exit(1);
  }
  
  while (1) {
    int sockfd;//就相当于NPC
    printf("Socket before accept.\n");//输出调试
    if (sockfd = accept(server_listen, NULL, NULL) < 0) {
      //参数表明不关注是哪个地址
      perror("accept");
      close(sockfd);
      continue;
    }
    //如果连接成功了
    //我现在想知道对方的名字，
    printf("Socket After accpet.\n");//输出调试
    char name[20] = {0};
    if (recv(sockfd， name, sizeof(name), 0) <= 0) {
      //如果接收名字失败
      close(sockfd);
      continue;
    }
    
    printf("Socket received.\n");//输出调试
    //如果成功收到名字了
    printf("name : %s\n", name);
    //但是这么做很像一个智障，因为如果对方不给我名字，我就会阻塞
    close(sockfd);
  }
  
  return 0;
}
```



## Client端

Clinet端也需要socket，但是不需要有门牌号(端口号)

```C
#include <stdio.h>
#incldue <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char **argv) {
  int sockfd, port;
  struct sockaddr_in server;
  if (argc != 3) {
    //2个参数分别是对方ip(大楼号)和端口号(门牌号)
    fprintf(stderr, "Usage : %s ip port\n", argv[0]);
    exit(1);
  }
  
  port = atoi(argv[2]);//将第二个参数转化为整形赋给port
  
  //对server结构体的处理
  server.sin_family = AF_INET;
  server.sin_port = htons(port);
  server.sin_addr.s_addr = inet_addr(argv[1]);//将字符串转为网络字节序的整形
  
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket");
    exit(1);
  }
  
  printf("Socket create.\n");//输出调试
  
  //主动连接
  if (connect(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0) {
    perror("connect");
    exit(1);
  }
  
  //发送数据，为一行字符串
  if (send(sockfd, "chenzheyu", sizeof("chenzheyu"), 0) < 0) {
    perror("send");
    exit(1);
  }
  close(sockfd);
  
  
  return 0;
}
```



但是这个server端还有bug。

如果一个Cilent端连上去之后，就不发送名字，并且不结束程序，那么Server端就会阻塞，别人连接也连不上！！