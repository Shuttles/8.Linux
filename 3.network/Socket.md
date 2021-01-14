

# socket编程入门：1天玩转socket通信技术（非常详细）

这个写得还可以！

http://c.biancheng.net/socket/

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

2. 14字节那个就是放的本机的ip地址和端口信息
+ 端口是自己的端口(门牌号)
   + 因为一个机器可能有多个ip地址
   + ip地址可以设为一个固定的地址，这就意味你只能接受从那个ip地址发过来的数据
+ ip地址一般都设置为`0.0.0.0`，表示任意ip地址发过来的数据都接受
   
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

2. 参数是==目的地端(你想连接的人)的端口号和ip地址==





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
    if ((sockfd = accept(server_listen, NULL, NULL)) < 0) {
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



## Client端de

Clinet端也需要socket，也需要有门牌号(端口号)，只不过client端的端口号不需要自己`bind`申请！而是由操作系统分配！！

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

接下来就使用多进程来优化server端

## 使用多进程优化Server端

在最前面讲到了进程是clone出来的，但是这是系统干的事情，系统留给用户去clone进程的是一个函数`fork`。

### fork

用来创建子进程的。

![img](https://wx2.sinaimg.cn/mw690/005LasY6gy1gd4rzoc2u5j311y0fsgns.jpg)

创建出来的进程与原来的进程相互独立！

系统经常用的`clone`函数如下

![img](https://wx4.sinaimg.cn/mw690/005LasY6gy1gd4s0weoubj31ou0rodyb.jpg)



但我们通常都用`fork`

![img](https://wx1.sinaimg.cn/mw690/005LasY6gy1gd4s2yy90rj31fo0u04qp.jpg)

![img](https://wx1.sinaimg.cn/mw690/005LasY6gy1gd4s93hlpnj31o80ekgy5.jpg)

1. 根据函数原型可知，返回值是一个`子进程的pid`，不需要传参数

2. 根据description可知，创建的进程被认为是子进程，调用这个函数的进程被认为是父进程。

   ***<u>并且在子进程复制完父进程之后</u>*，他俩在独立的内存空间上运行**

3. 根据返回值可知，子进程可以得到fork的返回值= 0，父进程中得到的返回值是子进程的pid。

   ***<u>由这个返回值就可以判断哪个是子进程哪个是父进程</u>***。



### 程序

```C
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
    if ((sockfd = accept(server_listen, NULL, NULL)) < 0) {
      //参数表明不关注是哪个地址
      perror("accept");
      close(sockfd);
      continue;
    }
    
    //使用fork创建子进程
    pid_t pid;
    if ((pid = fork()) < 0) {
      perror("fork");
      continue;
    }
    
    //因为子进程一开始是复制的父进程的内容，那么他俩可以做相同的事，但是我现在想让他俩做不同的事，就得判断哪个是子进程哪个是父进程
    if (pid == 0) {//如果创建子进程成功，那么在子进程中的pid是被设置为0的
      close(server_listen);
      char name[20] = {0};
      if (recv(sockfd, name, sizeof(name), 0) <= 0) {
        perror("recv");
        close(sockfd);
      }
      printf("Name = %s\n", name);
      eixt(0);//如果子进程在这不退出，就会继续在While循环里，这不是我们想要的
    }
    
    /*下面这段是原来没有优化过的
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
    */
  }
  
  return 0;
}
```



## 处理zombie进程

分析上面的代码，可以发现，会产生zombie进程

为什么会产生zombie进程呢？

### zombie进程产生的原因

> 在linux系统中，当用ps命令观察进程的执行状态时，经常看到某些进程的状态栏为defunct，这就是所谓的“僵尸”进程。“僵尸”进程是一个早已死亡的进程，但在进程表（processs table）中仍占了一个位置（slot）。由于进程表的容量是有限的，所以，defunct进程不仅占用系统的内存资源，影响系统的性能，而且如果其数目太多，还会导致系统瘫痪。
>
> 我们知道，每个进程在进程表里都有一个进入点（entry），核心程序执行该进程时使用到的一切信息都存储在***<u>进入点</u>***。当用ps命令察看系统中的进程信息时，看到的就是进程表中的相关数据。
>
> 所以，当一个父进程以fork()系统调用建立一个新的子进程后，核心进程就会在进程表中给这个子进程分配一个进入点，然后将相关信息存储在该进入点所对应的进程表内。这些信息中有一项是其父进程的识别码。
> ————————————————
> 版权声明：本文为CSDN博主「leon1741」的原创文章，遵循 CC 4.0 BY-SA 版权协议，转载请附上原文出处链接及本声明。
> 原文链接：https://blog.csdn.net/LEON1741/article/details/78142269

当调用`exit`命令结束子进程的时候，子进程是没有真正被销毁的，而是留下一个称为zombie进程的数据结构(系统调用exit的作用是使进程退出，但是也仅仅限于一个正常的进程变成了一个僵尸进程，并不能完全将其销毁)。

此时原来进程表中的数据会被该进程的退出码(exit code)、执行时所用的CPU时间等数据所取代，这些数据会一直保留到系统将它传递给它的父进程为止。

***<u>由此可见，zombie进程的出现时间是在子进程终止后，但是父进程尚未读取这些数据之前。</u>***

此时，该zombie子进程已经几乎放弃了所有的内存空间，没有任何可执行代码，也不能被调度，仅仅在进程列表中保留一个位置，记载该进程的退出状态信息供其他进程收集，除此之外，僵尸进程不再占有任何存储空间。它需要它的父进程来为他收尸。

如果它的父进程没有安装`SIGCHLD`信号处理函数调用`wait或waitpid()`等待子进程结束，也没有显式忽略该信号，那么它就一直保持zombie状态，如果这时候父进程结束了，那么init进程也会自动接收这个子进程，为他收尸，他还是能被清除掉的。***<u>但是如果父进程是一个循环，不会结束，那么子进程就会一直保持僵尸状态，这就是系统中为什么有时候会有很多僵尸进程的原因了。</u>***



### 如何杀死僵尸进程

1. 重启服务器

2. 找到该进程的父进程，将其杀掉

   如何找到该僵尸进程的父进程呢？

   `ps -ef | grep pid`





### 预防zombie进程

> + 在父进程创建子进程之前，就向系统申明自己并不会对这个子进程的exit动作进行任何关注行为，这样的话，子进程一旦退出后，系统就不会去等待父进程的操作，而是直接将该子进程的资源回收掉，也就不会出现僵尸进程了。具体的办法就是，在父进程的初始化函数中，调用这个函数：signal(SIGCHLD,SIG_IGN)
> + 如果上述语句没来得及调用，也有另外一个办法。那就是在创建完子进程后，用waitpid等待子进程返回，也能达到上述效果；
> + 如果上述两个办法都不愿意采用，那还有一招：在父进程创建子进程的时候，连续调用两次fork()，而且使紧跟的子进程直接退出，使其孙子进程成为孤儿进程，从而init进程将代替父进程来接手，负责清除这个孤儿进程。于是，父进程就无需进行任何的清理行为，系统会自动处理；
>   ————————————————
>   版权声明：本文为CSDN博主「leon1741」的原创文章，遵循 CC 4.0 BY-SA 版权协议，转载请附上原文出处链接及本声明。
>   原文链接：https://blog.csdn.net/LEON1741/article/details/78142269









## 代码封装

-把不同的部分封装到不同的部分中去



### 1.common目录

--公用的函数、头文件

#### 1.head.h

```c
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
```

