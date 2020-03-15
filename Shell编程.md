

脚本并不是程序！！！

是个文本，运行时由shell解释！！



shell是弱类型语言！(定义变量时可以没有类型，未初始化的变量初值是0)

# 变量与局部变量

## 变量的定义

```shell
a=12
a=helloworld
a='pwd'
a=$a:a
```

a到底是什么类型，取决于你怎么用它！！



## 局部变量

local a=12



## 特殊变量

### 位置变量

```shell
$0 ：获取当前执行shell脚本的文件名，包括路径。
$n ：获取当前执行脚本的第n个参数，n=1，2，...9，如果n>9，则需要将n使用大括号括起来；
$* ：获取当前shell的所有参数，将所有命令行参数视为单个字符串，相当于"$1$2$3"；
$# ： 得到执行当前脚本的参数个数；(#可用于求长度)
$@ ：获取这个程序所有参数，并保留参数之间的任何空白，相当于"$1" "$2" "$3"
```



### 状态变量

```shell
$? ： 判断上一指令是否成功执行，0为成功，非0为不成功
$$ ： 取当前进程的PID
$! ： 上一个指令的PID

//PID：procesID
```



### 变量，参数展开

```shell
${parameter:-word}如果变量未定义，则表达式的值为word
${parameter:=word}如果变量未定义，则设置变量的值为word，返回表达式的值也是word
${parameter:?word}用于捕捉由于变量未定义而导致的错误并退出程序。

```



### 字符串展开

```shell
${parameter/pattern/string}第一个匹配的字符串被替换
${parameter//pattern/string}匹配到的全部的字符串被替换
${parameter/#pattern/string} 字符串开头的替换
${parameter/%pattern/string} 字符串结尾的替换
${parameter,,} ${parameter}
```





# 输入输出

read [-options] [variable...]



echo string



printf





# 函数



#  流程控制





# 数组

```shell
declare -a a
	name[subscipt]=value
	name=()
```

