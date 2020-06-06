# Shell

## 一些概念

1. console：The operator's station of a mainfram. 大型机的工作台

   ![img](https://wx2.sinaimg.cn/mw690/005LasY6gy1gcpqp9mvxbj31400u01kx.jpg)

2. terminal：

   An electronic or eletromechanical device for entering data into a computer or a communication's system and displaying data received.Early terminals were called teletypes,later ones VDUs.

   ![img](https://wx1.sinaimg.cn/mw690/005LasY6gy1gcpqscbmx1j31ds0ti17t.jpg)

   > console和terminal的概念都源于大型机，console可以看作为一个特殊的terminal。现在用的一般都是引申义，一般情况下可以混用。

3. shell(贝壳)：

   + 人机交互的接口，是指“==提供使用者使用界面的软件==”， 是一种命令解释器。
   + 它给用户提供了一个输入命令并接受机器返回结果的界面。

   + GUI(图形化界面)也是一种shell



## Terminal Shell Bash关系

1. 在现在，terminal是用软件实现的。
2. terminal中，会运行一个shell软件来扩充为一个具备shell功能的程序。
3. shell有多种，bash是其中一种，还有zsh、sh

![img](https://wx3.sinaimg.cn/mw690/005LasY6gy1gcpqtj9bi0j31b60u016i.jpg)





# 相关文件



## 全局设置与个人设置

1. Linux是一个多任务多用户的OS，用户之间是隔离的
2. 全局设置就是如果程序开发做了修改，那么所有用户的程序都会改变

![img](https://wx1.sinaimg.cn/mw690/005LasY6gy1gcpr0wjvdqj31e90u07oc.jpg)

3. 例子

   ![img](https://wx3.sinaimg.cn/mw690/005LasY6gy1gcpr409cb7j31fk0u0ar9.jpg)

   ![img](https://wx4.sinaimg.cn/mw690/005LasY6gy1gcpr46rmfcj31id0u0arh.jpg)

   + bashrc的rc就是resource，即启动bash需要调用的系统资源，是一个配置文件
   + bash.profile中的profile是资料、档案的意思，是针对于用户登录的，你运行bash就会运行这个登录文件
   + bash.logout中的logout是注销的意思，是用户退出的时候会调用执行的文件





# Homework

根据上面讲到的文件，再去了解一下Linux的变量，写一个计算用户在线时长的程序。

就是当你退出shell的时候，显示你从登录到退出之间在线的时间。(精确到分钟)



可以以ssh本地为例，即`ssh localhost`





解：

1. ==其实思路很简单，用一个变量记录用户登录时的time，用另一个变量记录用户logout时的time，最后相减就行了。==

2. 如何获得登录时的时间呢？

   联想到在用户登录时，会启动`.bashrc`文件，所以可以在这个文件中做手脚。

   即

   在`.bashrc`中添加一行`LoginTime=`date +%s`  `，获得从计算机元年开始至今经过的秒数

   + ``是命令替换符，将其中的命令先执行，然后将结果返回
   + 想要输出正常的时间，命令是`date +%H:%M:%S`

3. 如何获得退出的时间呢？

   + 这就很简单了，在`.bash_logout`中定义一个`LogoutTime=`date +%s` `

4. 最后计算并输出

   + 还是在`.bash_logout`中定义一个`TimeSec=$[ ${LogoutTime} - ${LoginTime} ]`
   + 接着`Time=$[ ${TimeSec} / 60 ]`
   + 最后`echo "Login for ${Time} min."`





