| 命令   | 功能                             |
| ------ | -------------------------------- |
| uptime | 打印系统运行时长和平均负载       |
| w      | 当前登录用户列表及正在执行的任务 |
| who    | 显示当前登录系统的用户信息       |
| whoami | 打印当前有效的用户名称           |
| last   | 显示用户最近登录信息             |
| uname  | 打印当前系统信息                 |
| date   | 显示或设置系统时间与日期         |
| cal    | 显示日历                         |
|        |                                  |

1. 为什么要有w和who这些命令？

   因为linux是个多用户系统！在绝大多数情况下，一个系统是存在很多用户的！



# uptime

```shell
uptime [-phsv]
	打印系统运行时长 和 平均负载(最近1min,5min,15min)
	-s 开机的时间
	-p 开机后经过多少时间
```

![img](https://wx4.sinaimg.cn/mw690/005LasY6gy1gcug464lg9j31m009qn4s.jpg)



# w

```shell
w [-husfV] <user> <...>
	#获取当前登录用户和正在执行的进程
	#PS:包含了uptime的信息！！！
	-h 不打印头信息
	-s 使用短输出格式
	-f 切换显示FROM项，默认显示
```

![img](https://wx4.sinaimg.cn/mw690/005LasY6gy1gcugad1lelj31g6076tf7.jpg)

![img](https://wx3.sinaimg.cn/mw690/005LasY6gy1gcugakrgorj31jq0cggsz.jpg)



# who

```shell
who [-Huq] <file | arg1 arg2>
	#显示当前登录系统的用户信息
	-H 显示各栏的标题信息列
	-u 显示空置时间
	-q 只显示登入系统的用户名称和总人数
```

![img](https://wx4.sinaimg.cn/mw690/005LasY6gy1gcugf32kbsj315k0akwk4.jpg)



# whoami

```shell
#可以这么看，whoami是不严肃地问，who am i是严肃地问
```

![img](https://wx2.sinaimg.cn/mw690/005LasY6gy1gcughbobj5j30jm062dgh.jpg)

![img](https://wx1.sinaimg.cn/mw690/005LasY6gy1gcugjuuqf9j313i08ejv8.jpg)



# last

```shell
last [-fnx] <username...> <tty...>
	#显示用户最近登录信息
	-f "file" 指定记录文件
	-n "num" 指定输出记录数
	-x 显示系统关机，重新关机等信息
```

![img](https://wx3.sinaimg.cn/mw690/005LasY6gy1gcugo90h7tj31kg0guk7c.jpg)



# uname

```shell
uname [-amnvpio]
	#打印当前系统信息
	-a 全部信息
	-m 电脑类型
	-n 网络上主机的名称
	-v os版本
	-p 处理器类型
	-i 硬件平台
	-o 操作系统名称
```

![img](https://wx1.sinaimg.cn/mw690/005LasY6gy1gcugtk98c2j319g0buwgx.jpg)





# date

```shell
date [-dsu] <参数>
	#显示或设置系统时间与日期
	-d "string" 显示字符串所指的日期
	-s "string" 设置时间
	-u 显示GMT(如果是CST的情况下)
参数
	<+日期格式> 显示使用的日期格式
```

![img](https://wx4.sinaimg.cn/mw690/005LasY6gy1gcugz2k8d6j30r80byaby.jpg)



# cal

```shell
cal [option] <参数>
	#显示日历
选项
	-1,-3 显示最近一个月和三个月的日历
	-j 显示一年的第几天
	-y 显示当前年
参数
	月：指定月份
	年：指定年份
```

![img](https://wx2.sinaimg.cn/mw690/005LasY6gy1gcuh39dmrfj30oa0nidnu.jpg)

