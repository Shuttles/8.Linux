

==到底有没有真正搞懂IO多路复用，可以问自己3个问题：==

1. 为什么会出现这项技术？
2. IO多路复用三个API之间的区别？
3. IO多路复用到底复用的是什么？





# 0.简介

1. 之前的并发都是用多进程/多线程写的

2. **多进程**

   + 受限于CPU核心数
   + 受限于进程数量
   + 进程创建/销毁代价太高
   + 进程内存隔离导致进程间通信代价高

3. **多线程**

   + 受限于CPU核心数
   + 进程创建/销毁代价也不低

4. **共同致命缺点**：

   是**阻塞IO**

   + 阻塞IO**响应能力**比较低
   + 如果换成非阻塞IO，就没办法感知IO了，只能不断轮询，效率也比较低

5. **IO多路复用**

   + 可以感知IO是否可读/可写/出错，判断IO是否就绪
   + 单线程，不受限于CPU核心数



# 1.select

## 1.1API

1. **简介及头文件**

   ![img](https://wx1.sinaimg.cn/mw690/005LasY6ly1gv9dygqxnij60p608zgmt02.jpg)

2. **函数原型**

   ![img](https://wx2.sinaimg.cn/mw690/005LasY6ly1gv9e2h03z8j60ho04mq3o02.jpg)

   ![img](https://wx3.sinaimg.cn/mw690/005LasY6ly1gv9e2lw0hrj60oh04h0ub02.jpg)
   
3. **描述**

   + 简介

     ![img](https://wx3.sinaimg.cn/mw690/005LasY6ly1gva844p6nlj60n405mjt702.jpg)

   + 监听的具体含义

     ![img](https://wx4.sinaimg.cn/mw690/005LasY6ly1gva8afp2xfj60ld059mz902.jpg)

   + 阻塞何时终止

     ![img](https://wx4.sinaimg.cn/mw690/005LasY6ly1gva8f5huj2j60lf05mmy102.jpg)

   + 返回之后

     ![img](https://wx4.sinaimg.cn/mw690/005LasY6ly1gva8cb5ms0j60ld02174x02.jpg)

     ![img](https://wx1.sinaimg.cn/mw690/005LasY6ly1gva8cdh74ij60la02qjs602.jpg)

   + timeval

     ![img](https://wx3.sinaimg.cn/mw690/005LasY6ly1gva8ihqgk4j60l309575402.jpg)

4. 其他：

   见man手册



主要开销：

1. 多个文件描述符只有**一次 select 的系统调用** + **n 次**就绪状态的文件描述符的 **read 系统调用**



==**几个问题：**==

1. select 调用需要传入 fd 数组，需要**拷贝一份到内核**（交给内核去**遍历**监听），高并发场景下这样的**拷贝消耗的资源是惊人的**。（可优化为不复制）
2. select 在内核层仍然是通过**遍历**的方式检查文件描述符的就绪状态，是个同步过程，只不过**无系统调用切换上下文的开销**。（内核层可优化为异步事件通知）
3. select 仅仅返回可读文件描述符的个数，**具体哪个可读还是要用户自己遍历**。（可优化为只返回给用户就绪的文件描述符，无需用户做无效的遍历）





# 2.poll

1. 它和 select 的主要区别就是，==去掉了 select 只能监听 1024 个文件描述符的限制==。

2. select 使用固定长度的 BitsMap（**数组**），表示文件描述符集合，而且所支持的文件描述符的个数是有限制的，在 Linux 系统中，由内核中的 FD_SETSIZE 限制， 默认最大值为 `1024`，只能监听 0~1023 的文件描述符。

   poll 不再用 BitsMap 来存储所关注的文件描述符，取而代之用动态数组，以**链表**（==是这样吗？为什么poll参数里还是数组呢？==）形式来组织，突破了 select 的文件描述符个数限制，当然还会受到系统文件描述符限制。

3. 但是 poll 和 select 并没有太大的本质区别，**都是使用「线性结构」存储进程关注的 Socket 集合，因此都需要遍历文件描述符集合来找到可读或可写的 Socket，时间复杂度为 O(n)，而且也需要在用户态与内核态之间拷贝文件描述符集合**，这种方式随着并发数上来，性能的损耗会呈指数级增长。



## 2.1API

1. **简介及头文件**

   ![img](https://wx1.sinaimg.cn/mw690/005LasY6ly1gva8nqc4baj60mu04aaae02.jpg)

2. **函数原型**

   ![img](https://wx1.sinaimg.cn/mw690/005LasY6ly1gva8nt5attj60gb014dft02.jpg)

   ![img](https://wx4.sinaimg.cn/mw690/005LasY6ly1gva8qazd6dj60n103u75c02.jpg)

3. **描述**

   + 参数

     ![img](https://wx3.sinaimg.cn/mw690/005LasY6ly1gva8x8k4cxj60n208tjt602.jpg)

     ![img](https://wx1.sinaimg.cn/mw690/005LasY6ly1gva99j3aosj60la0bs42s02.jpg)

   + events revents

     ![img](https://wx3.sinaimg.cn/mw690/005LasY6ly1gva9bfm36wj60lj0cc40p02.jpg)



# 3.epoll

## 3.1API

**epoll整体man手册**

![img](https://wx4.sinaimg.cn/mw690/005LasY6ly1gva9itj9haj60nb0fd77y02.jpg)



**epoll_create:**

1. **简介及头文件**

   ![img](https://wx2.sinaimg.cn/mw690/005LasY6ly1gva9nhxdduj60n904fmxm02.jpg)

2. **函数原型**

   ![img](https://wx2.sinaimg.cn/mw690/005LasY6ly1gva9nhxdduj60n904fmxm02.jpg)

   ![img](https://wx2.sinaimg.cn/mw690/005LasY6ly1gva9pzw3arj60n101zmxh02.jpg)

3. **描述**

   ![img](https://wx1.sinaimg.cn/mw690/005LasY6ly1gva9qw4q63j60nd06ftaw02.jpg)

   



**epoll_ctl:**

1. **简介及头文件**

   ![img](https://wx2.sinaimg.cn/mw690/005LasY6ly1gva9t1lhb3j60na04eq3d02.jpg)

2. **函数原型**

   ![img](https://wx3.sinaimg.cn/mw690/005LasY6ly1gva9t7mtnnj60j301a0sr02.jpg)

   ![img](https://wx3.sinaimg.cn/mw690/005LasY6ly1gva9tagokxj60n102474k02.jpg)

3. **描述**

   ![img](https://wx3.sinaimg.cn/mw690/005LasY6ly1gva9xg5r9sj60nb0cmacq02.jpg)

   ![img](https://wx3.sinaimg.cn/mw690/005LasY6ly1gva9xk7d20j60lp0aiacc02.jpg)



**epoll_wait:**

1. **简介及头文件**

   ![img](https://wx2.sinaimg.cn/mw690/005LasY6ly1gvaa73ikc0j60ng04hdgc02.jpg)

2. **函数原型**

   ![img](https://wx4.sinaimg.cn/mw690/005LasY6ly1gvaajv2d25j60fe01g74d02.jpg)

   ![img](https://wx3.sinaimg.cn/mw690/005LasY6ly1gvaak02q26j60n803cmy102.jpg)

3. **描述**

   ![img](https://wx4.sinaimg.cn/mw690/005LasY6ly1gvaap4k9ouj60m303ydgw02.jpg)

   不过不是很懂events是传入参数还是传出参数？

   ![img](https://wx3.sinaimg.cn/mw690/005LasY6ly1gvaaswalayj60kb02x3zb02.jpg)

   看上去好像是**传出参数**

   





## 3.2两种模式

1. epoll有EPOLLLT和EPOLLET两种触发模式，LT是默认的模式（**水平触发**），ET是“高速”模式（**边缘触发**）。
2. LT模式下，**只要这个fd还有数据可读，每次 epoll_wait都会返回它的事件，提醒用户程序去操作**
3. ET模式下，**它只会提示一次，直到下次再有数据流入之前都不会再提示了，无论fd中是否还有数据可读**。所以在ET模式下，read一个fd的时候一定要把它的buffer读完，或者遇到EAGAIN错误



## 3.3原理

1. epoll 在内核里使用**红黑树来跟踪进程所有待检测的文件描述字**
2. epoll 使用事件驱动的机制，内核里**维护了一个链表来记录就绪事件**
3. epoll 的方式即使监听的 Socket 数量越多的时候，效率不会大幅度降低，能够同时监听的 Socket 的数目也非常的多了，**上限**就为**系统定义的进程打开的最大文件描述符个数**。因而，**epoll 被称为解决 C10K 问题的利器**。





==对select进行改进==

1. 内核中保存一份fd集合，**无需用户每次都重新传入**，只需要告诉内核修改的部分即可
2. 内核不再通过轮询的方式找到就绪的fd，而是**通过异步IO事件唤醒**
3. 内核仅会**将有IO事件的fd返回给用户**，用户无需遍历整个fd集合。





# 说明

1. 另外，使用 I/O 多路复用时，最好搭配**非阻塞 I/O** 一起使用，Linux 手册关于 select 的内容中有如下说明：

   > Under Linux, select() may report a socket file descriptor as "ready for reading", while nevertheless a subsequent read blocks. This could for example happen when data has arrived but upon examination has wrong checksum and is discarded. There may be other circumstances in which a file descriptor is spuriously reported as ready. Thus it may be safer to use O_NONBLOCK on sockets that should not block.

   我谷歌翻译的结果：

   > 在Linux下，select() 可能会将一个 socket 文件描述符报告为 "准备读取"，而后续的读取块却没有。例如，当数据已经到达，但经检查后发现有错误的校验和而被丢弃时，就会发生这种情况。也有可能在其他情况下，文件描述符被错误地报告为就绪。因此，在不应该阻塞的 socket 上使用 O_NONBLOCK 可能更安全。

   简单点理解，就是**多路复用 API 返回的事件并不一定可读写的**，如果使用阻塞 I/O， 那么在调用 read/write 时则会发生程序阻塞，因此最好搭配非阻塞 I/O，**以便应对极少数的特殊情况**。





# 比较

1. select需要打开文件，os对每一个进程能打开的文件是有限制的，默认最大值1024，所以select最高并发是1024个
2. select中有三个set，而poll中只有一个数组和两组events，更进一步，epoll中只有一组events
3. select只能监听read、write、异常事件，而poll和epoll**可以监听更多事件**。





# 技术总结

1. 多路复用有三个函数

   + 最开始是 select
   + 然后又发明了 poll 解决了 select 文件描述符的限制
   + 然后又发明了 epoll 解决 select 的三个不足。

2. 所以，IO 模型的演进，其实就是时代的变化，**倒逼着操作系统将更多的功能加到自己的内核**而已。

3. 好多文章说，多路复用之所以效率高，是因为**用一个线程就可以监控多个文件描述符**。

   **这显然是不对的**，是知其然而不知其所以然，多路复用产生的**效果，完全可以由用户态去遍历文件描述符并调用其非阻塞的 read 函数实现**。而多路复用**快的原因**在于，==操作系统提供了这样的系统调用，使得原来的 while 循环里多次系统调用，变成了一次系统调用 + 内核层遍历这些文件描述符==。



# 白话总结

==阻塞IO --> 多线程 --> 非阻塞IO(单线程监听多fd) --> IO多路复用==



1. 一切的开始，都起源于这个 read 函数是操作系统提供的，而且是阻塞的，我们叫它 **阻塞 IO**。

2. 为了破这个局，程序员**在用户态通过多线程来防止主线程卡死**。

3. 后来操作系统发现这个需求比较大，于是在操作系统层面提供了**非阻塞**的 read 函数，这样程序员就**可以在一个线程内完成多个文件描述符的读取**，这就是 **非阻塞 IO**。

   但多个文件描述符的读取就需要遍历，当高并发场景越来越多时，**用户态遍历的文件描述符也越来越多，相当于在 while 循环里进行了越来越多的系统调用**。

4. 后来操作系统又发现这个场景需求量较大，于是又在操作系统层面提供了这样的遍历文件描述符的机制，这就是 **IO 多路复用**。

5. 多路复用有三个函数，最开始是 select，然后又发明了 poll 解决了 select 文件描述符的限制，然后又发明了 epoll 解决 select 的三个不足。

   ------

   ==所以，IO 模型的演进，其实就是时代的变化，倒逼着操作系统将更多的功能加到自己的内核而已。==





# 参考文献

1. 你管这破玩意叫 IO 多路复用？（低并发编程）

   https://mp.weixin.qq.com/s/YdIdoZ_yusVWza1PU7lWaw

2. 图解 | 深入揭秘 epoll 是如何实现 IO 多路复用的！

   https://mp.weixin.qq.com/s?__biz=MjM5Njg5NDgwNA==&mid=2247484905&idx=1&sn=a74ed5d7551c4fb80a8abe057405ea5e&chksm=a6e304d291948dc4fd7fe32498daaae715adb5f84ec761c31faf7a6310f4b595f95186647f12&scene=21#wechat_redirect

3. 这次答应我，一举拿下IO多路复用！（小林coding）

   https://www.zhihu.com/question/32163005/answer/1802684879

4. 彻底理解IO多路复用

   https://zhuanlan.zhihu.com/p/150972878

5. 极客时间-网络编程实战

   https://time.geekbang.org/column/article/143245
   
   