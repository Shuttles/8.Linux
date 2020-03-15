# Shell

## 一些概念

1. console：The operator's station of a mainfram. 大型机的工作台

   ![img](https://wx2.sinaimg.cn/mw690/005LasY6gy1gcpqp9mvxbj31400u01kx.jpg)

2. terminal：

   An electronic or eletromechanical device for entering data into a computer or a communication's system and displaying data received.Early terminals were called teletypes,later ones VDUs.

   ![img](https://wx1.sinaimg.cn/mw690/005LasY6gy1gcpqscbmx1j31ds0ti17t.jpg)

3. shell(贝壳)：人机交互的接口，是指“提供使用者使用界面的软件”， 是一种命令解释器。

   GUI(图形化界面)也是一种shell



## Terminal Shell Bash关系

1. 在现在，terminal是用软件实现的。
2. terminal中，会运行一个shell软件。
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