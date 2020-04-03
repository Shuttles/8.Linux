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