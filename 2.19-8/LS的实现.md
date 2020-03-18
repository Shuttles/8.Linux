# 第一阶段(ls)

1. 第一阶段即实现最简单的ls--在终端中显示本目录下的非隐藏文件名

要用到的函数如下

## opendir

1. 我们要查看某个目录下的所有文件，首先必须打开这个目录，打开还不够，还得读，读完还得关闭，所以就必须得用到opendir。

2. ![img](https://wx3.sinaimg.cn/mw690/005LasY6gy1gcvt3u2q0ej31e10u0ka6.jpg)

   ![img](https://wx3.sinaimg.cn/mw690/005LasY6gy1gcvt6qlu3ij31mo0my1kx.jpg)

3. 通过函数原型，我们可以知道，传入一个目录名(字符串类型)，我们就可以打开一个目录。通过返回值我们可以知道返回的是一个文件指针，如果不成功会返回NULL。

   PS：

   + 注意RETURN VALUE中的errno！！！

     `errno` 是一个全局的错误变量，每一个错误都会有一个明确的含义，比如说错误13就是权限错误。

   + 那么问题来了，如何通过错误代号了解错误类型呢？

     通过函数`perror`即可！

     ![img](https://wx1.sinaimg.cn/mw690/005LasY6gy1gcvth8tcayj31ip0u0nb1.jpg)

     ![img](https://wx2.sinaimg.cn/mw690/005LasY6gy1gcvthwcbqcj31l80pwnmj.jpg)

     `perror`可以根据全局变量`errno`的值来打印错误信息！

   + 也就是说，如果`opendir`的返回值是`null`的话，就可以调用`perror`来打印错误信息！！

4. 通过description我们可以知道打开目录后，目录流被定位在入口处！

## readdir

1. 通过`opendir`我们可以打开一个目录，如果打开成功，会返回一个`DIR`类型的指针，那么这个指针是什么呢？我们如何通过它来读目录中的文件呢？

   这就要用到`readdir`了！

2. ![img](https://wx4.sinaimg.cn/mw690/005LasY6gy1gcvtobsnypj31ls0sinb2.jpg)

   ![img](https://wx4.sinaimg.cn/mw690/005LasY6gy1gcvtudixojj31g60u07wh.jpg)

   ![img](https://wx4.sinaimg.cn/mw690/005LasY6gy1gcvu25795gj31lq0pa1cv.jpg)

3. 通过函数原型我们可以看到，通过`opendir`返回的`DIR`类型的指针就应该传给`readdir`来进行处理。

4. 通过函数原型我们还可以看到，`readdir`返回的是`struct dirent`类型的指针。

5. 通过description我们可以看到，每次读完之后，`readdir`返回的是该目录流下一个条目的指针，由此我们可以想到应该循环读，直到`readdir`返回`NULL`为止。

6. 我们来看看这个`struct dirent`结构体类型

   + `ino_t`即`inode_t`，是一个自定义的类型，并且以`t`结尾的类型往往都是整形！但是为了能让我们更直观地看出这是用来存储什么变量的，所以才自定义的。
   + 注意到`d_name[256]`，这就是我们想要的文件名！！



## closedir

1. 打开完目录，读完目录，之后，一定得关闭！

   因此，就必须要`closedir`！

2. ![img](https://wx3.sinaimg.cn/mw690/005LasY6gy1gcvu6hip8yj31h80u0wxu.jpg)

3. 通过description可知，只要调用了这个函数，文件指针就不在可得了，至此，才算完成关闭文件！



代码如下

```c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types/h>

void do_ls(char[]);

int main(int argc, char *argv[]) {//第二个参数为目录名(可能为多个)
  if (argc == 1) {//这个c程序本身是第一个参数，也就是说如果ls后面不加参数的的话
    do_ls(".");//对本目录执行ls
  } else {
    while (--argc) {
      printf("%s:\n", *++argv);//先打印目录名
      do_ls(*argv);
      (argc == 1) || printf("\n");//最后一个目录内的文件名打印完之后不留空行了
    }
  }
  return 0;
}

void do_ls(char dirname[]) {
  DIR *dirp;
  struct dirent *direntp;//文件基本信息
  if ((dirp = opendir(dirname)) == NULL) {
    perror("opendir"); //如果打开出错，调用perror函数打印错误信息
    return ;
  }
  while ((direntp = readdir(dirp) != NULL)) {
    if (direntp->d_name[0] == '.') continue;//如果是隐藏文件，不输出
    printf("%s\n", direntp->d_name);//现在是每个目录下的文件都输出在一列中
  }
  closedir(dirp);
  printf("\n");
}
```

完成后的效果

![img](https://wx1.sinaimg.cn/mw690/005LasY6gy1gcvxgu24h0j312y0u0npd.jpg)

实际的ls

![img](https://wx2.sinaimg.cn/mw690/005LasY6gy1gcvxi9a6hdj32680hctl7.jpg)



# 第二阶段(分列输出)

1. 开始的思路：

   可以通过循环，先看一列装不装得下，如果装得下再看两列装不装得下，依次类推直至装不下为止。

2. 通过分析，如果采用上述思路，必须得获得当前terminal的宽度。

   如何获取呢？

   



## stat

1. 我们还需要知道每个文件的状态！

2. ![img](https://wx4.sinaimg.cn/mw690/005LasY6gy1gcvue4hh0wj31fi0u0qpm.jpg)

   ![img](https://wx2.sinaimg.cn/mw690/005LasY6gy1gcvui6vnz2j31m80n01e8.jpg)

   ![img](https://wx1.sinaimg.cn/mw690/005LasY6gy1gcvukzwfxfj31gn0u0njy.jpg)

   ![img](https://wx1.sinaimg.cn/mw690/005LasY6gy1gcvuomk7o9j31aq0d4tlc.jpg)

3. 通过函数原型可以知道，`buf`是一个传出参数。

4. 通过description可以知道，我们可以通过`stat`来获取文件的信息。

5. <u>***通过description还可以知道，传出参数`buf`的具体的结构体类型中有哪些信息。***</u>

   <u>***比如包含该文件的设备、inode号、文件权限、硬连接数、uid、gid、总大小、blocksize、ctime、atime、mtim***</u>



## getpwuid

1. 我们需要通过拿到的uid,gid获取名字！

2. ![img](https://wx3.sinaimg.cn/mw690/005LasY6gy1gcvus0k73nj31e60u0ata.jpg)

   ![img](https://wx1.sinaimg.cn/mw690/005LasY6gy1gcvuuimdrmj31jt0u01kx.jpg)

3. 通过函数原型可知，通过一个`uid`我们可以获得一个`passwd`结构体。

4. `passwd`结构体可知，我们可以获得`username`。





## getgrgid

1. 我们还需要获得gid组名！

2. ![img](https://wx4.sinaimg.cn/mw690/005LasY6gy1gcvv7xha37j31dq0u0h4l.jpg)

   ![img](https://wx3.sinaimg.cn/mw690/005LasY6gy1gcvv8z45h3j31ka0u0qqy.jpg)

   

3. 通过这个函数由gid获得组名！







接下来的问题就剩了如何展示文件类型和权限了！

还是得通过`stat`函数！

![img](https://wx1.sinaimg.cn/mw690/005LasY6gy1gcvvgis10uj31dv0u0e81.jpg)

通过&操作可以获得文件类型！



但现在有更加nb的操作来判断文件类型

![img](https://wx3.sinaimg.cn/mw690/005LasY6gy1gcvvil3w2dj31a30u019f.jpg)

这也是stat中的！！



关于文件权限

![img](https://wx1.sinaimg.cn/mw690/005LasY6gy1gcvvjx8jxzj314h0u0ty6.jpg)

也是在stat中！都是通过处理mode成员得到的！







排序问题！(根据文件名字典序)



放几列？

先一列，再往上加，看看行不行！