





# 2.Share_Memory

## 用到的函数

### shmge

![img](https://wx3.sinaimg.cn/mw690/005LasY6gy1gdisfxtcbpj31ew0u0nh5.jpg)

![img](https://wx4.sinaimg.cn/mw690/005LasY6gy1gdisgpjperj31js04qdjj.jpg)

1. 如果`key`对应的共享内存已经存在，那就返回共享内存id；否则就会根据`key`创建一个共享内存段并返回其id。
2. ==flag？？？？？？==
3. 那么问题来了，key从哪儿来呢？



### ftok

![img](https://wx2.sinaimg.cn/mw690/005LasY6gy1gdisnm3bkhj31gm0poqho.jpg)

1. 一般情况下，我们第一个参数传的就是当前工作目录，第二个参数就是两个进程相互协商的同一个id
2. 也就是说，如果两个参数相同，那么调用这个函数就可以获得同一个`key`



### shmat

![img](https://wx3.sinaimg.cn/mw690/005LasY6gy1gdistb808oj31fd0u0b17.jpg)

1. 如果第二个参数为0，那么系统就会自己给你找一个内存地址
2. ==flag???????==
3. 经过实践证明，shmat分配的地址是没有初始化的！！









## 亲缘关系进程间内存共享

1. ***<u>通过信号传递来保证多个进程不同时访问同一块共享内存！！！</u>***

```c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <signal.h>

char *share_memory = NULL;

void print(int signum) {
    printf("<parent> : %s\n", share_memory);
}

int main() {
    pid_t pid;
    int shmid;
    key_t key = ftok(".", 198);//获得key
    
    //创建4096字节的共享内存
    if ((shmid = shmget(key, 4096, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        return 1;
    }
    
    //获得共享内存的地址
    if ((share_memory = shmat(shmid, 0, 0)) <= 0) {
        perror("shmat");
        return 1;
    }
    
    if ((pid = fork()) < 0) {
        perror("fork");
        return 1;
    }
    
    if (pid == 0) {
        while (1) {
            printf("<child> : ");
            scanf("%[^\n]s", share_memory);
            getchar();
            //给父进程发送SIGUSR2信号
            kill(getppid(), SIGUSR2);
        }
    } else {
        while (1) {
            signal(SIGUSR2, print);//通过信号传递来保证不同时访问共享内存！！
        }
    }
    
    return 0;
}
```





## 非亲缘关系间内存共享

主要思想

1. 还是通过信号传递来控制！

2. 如何得到父进程pid呢？

   将共享内存里存结构体即可！

### Client端



```c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>

struct Shm {
    pid_t pid;
    char buff[1024];
};

struct Shm *share_memory = NULL;

int main() {
    int shmid;
    key_t key = ftok(".", 198);
    
    if ((shmid = shmget(key, sizeof(struct Shm), IPC_CREAT | 0666)) < 0) {
        //注意第二个参数变了！！！！！不再是4096！！
        perror("shmget");
        return 1;
    }
    
    if ((share_memory = (struct Shm *)shmat(shmid, 0, 0)) <= 0) {
        perror("shmat");
        return 1;
    }
    
    //因为未初始化内存空间，所以必须清掉脏数据
    memset(share_memory, 0, sizeof(struct Shm));
    
    printf("%d\n", share_memory->pid);
    
    while (1) {
        printf("<client> : ");
        scanf("%[^\n]s", share_memory->buff);
        getchar();//吞回车
        if (share_memory->pid == 0) {
            printf("<Error> : 服务器没上线！\n");
        } else {
            kill(share_memory->pid, SIGUSR2);//信号传递！！！
        }
    }
    return 0;
}
```





### Server端



```c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>

struct Shm {
    pid_t pid;
    char buff[1024];
};

struct Shm *share_memory = NULL;


//信号处理函数(handler)的定义
void print(int signum) {
    printf("<server> : %s\n", share_memory->buff);
}

int main() {
    int shmid;
    key_t key = ftok(".", 198);
    
    if ((shmid = shmget(key, sizeof(struct Shm), IPC_CREAT | 0666)) < 0) {
        //注意第二个参数变了！！！！！不再是4096！！
        perror("shmget");
        return 1;
    }
    
    printf("After shmget!\n");
    
    if ((share_memory = (struct Shm *)shmat(shmid, 0, 0)) <= 0) {
        perror("shmat");
        return 1;
    }
    
    printf("After shmat!\n");
    
   	share_memory->pid = getpid();
    
    while (1) {
        signal(SIGUSR2, print);//通过信号传递来保证不同时访问共享内存！！
        system(pause);//暂停，防止消耗CPU！！
    }
    return 0;
}
```



1. 这个程序有个bug

   就是如果client端后上线，就会清空pid！！！这样就感知不到server端了！