





# 2.Share_Memory

## 2.1用到的函数

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









## 2.2亲缘关系进程间内存共享

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





## 2.3非亲缘关系间内存共享

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





## 2.4互斥锁

互斥锁只有线程锁，没有进程锁！！

因此，我们只能***<u>通过线程锁来给进程</u>***加锁！！(通过==属性设置==)



> 互斥锁（英語：Mutual exclusion，缩写Mutex）是一种用于==多线程==编程中，防止两条线程同时对同一公共资源进行读写的机制。该目的通过将代码切片成一个一个的临界区域达成。临界区域指的是一块对公共资源进行访问的代码，并非一种机制或是算法。一个程序、进程、线程可以拥有多个临界区域，但是并不一定会应用互斥锁
>
> --维基百科

### 相关函数(pthread_mutex系列)

#### mutex

![img](https://wx3.sinaimg.cn/mw690/005LasY6gy1gdkf7u6bpqj31lg0qok6x.jpg)

![img](https://wx3.sinaimg.cn/mw690/005LasY6gy1gdkfhd7fd1j31l00scb29.jpg)

![img](https://wx1.sinaimg.cn/mw690/005LasY6gy1gdkfyrtmlej31ik07214o.jpg)

![img](https://wx2.sinaimg.cn/mw690/005LasY6gy1gdkg0dt5vaj31j40dgq9o.jpg)



SYNIOSIS

1. 可以快速声明并初始化一个线程锁

2. 由函数原型可知，第一个参数是你声明的锁的地址，第二个参数是你声明的锁的属性的地址！

   PS：==表面上mutex的类型是一个整形，但是底层一定是封装的结构体！！==



DESCRIPTION

1. ***<u>由第二段可知，一个线程如果锁了一块资源，那么别的线程想访问时会被挂起，直到该锁被unlock，然后这个线程才会被锁上，接着刚刚挂起的线程来访问这块资源！</u>***
2. 由第三段可知，如果第二个参数attr为NULL，那么就会被设置为默认值。
3. 第六段是核心！(其实就是解释的第一点)



EXAMPLE

妙！不愧是最佳示例！



#### mutexattr

![img](https://wx1.sinaimg.cn/mw690/005LasY6gy1gdkg6e81rqj31ly0isk1k.jpg)

![img](https://wx2.sinaimg.cn/mw690/005LasY6gy1gdkg9zjs0nj31kg0oa4qp.jpg)

![img](https://wx4.sinaimg.cn/mw690/005LasY6gy1gdkgat60tsj31li0mkk6x.jpg)





#### setpshared

![img](https://wx4.sinaimg.cn/mw690/005LasY6gy1gdkgkmax77j31ou0j8amz.jpg)



#### getpshared

![img](https://wx4.sinaimg.cn/mw690/005LasY6gy1gdkglxpoosj31ns0ieaoi.jpg)

![img](https://wx2.sinaimg.cn/mw690/005LasY6gy1gdkgmmpe97j31p20gi1ff.jpg)

![img](https://wx3.sinaimg.cn/mw690/005LasY6gy1gdkgnld580j31oe07mwmu.jpg)



### 如何与进程联系？

1. 由定义可知，在一个进程中的多线程都能感知到该mutex的存在，但是多进程是感受不到别的进程里的线程锁的！
2. 因此就得做一些骚操作了！
   + 把锁定义在`share_memory`中！
   + 设置`pshared`!



### 互斥锁应用--实现数字累加

共享内存+互斥锁

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <pthread.h>
#include <string.h>

struct Shm {
    int now;
    int sum;
    pthread_mutex_t mutex;//线程锁
};

struct Shm *share_memory = NULL;

void do_add(int max, int x) {
    while (1) {
        pthread_mutex_lock(&share_memory->mutex);
        printf("<%d> : %d %d\n", x, share_memory->now, share_memory->sum);
        
        if (share_memory->now > max) {
            //记得要关锁！！
            pthread_mutex_unlock(&share_memory->mutex);
            break;
        }
        
        share_memory->sum += share_memory->now;
        share_memory->now++;
        
        pthread_mutex_unlock(&share_memory->mutex);
    }
    return ;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage : %s max ins\n", argv[0]);
        return 1;
    }
    
    int max, ins;
    max = atoi(argv[1]);
    ins = atoi(argv[2]);
    
    pid_t pid;
    int shmid;
    key_t key = ftok(".", 198);
    
    if ((shmid = shmget(key, sizeof(struct Shm), IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        return 1;
    }
    if ((share_memory = (struct Shm *)shmat(shmid, 0, 0)) == NULL) {
        perror("shmat");
        return 1;
    }
    
    memset(share_memory, 0, sizeof(struct Shm));
    
    //下面这段就是核心操作！
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);//设置进程间共享！
    
    pthread_mutex_init(&share_memory, &attr);
    
    int x = 0;
    for (int i = 0; i < ins; i++) {
        if ((pid = fork()) < 0) {
            perror("fork");
            return 1;
        }
        if (pid == 0) {
            x = i;
            break;
        }
    }
    
    if (pid == 0) {
        do_add(max, x);
        printf("<%d> exit!\n", x);
        exit(0);
    }
    
    while (ins) {
        wait(NULL);
        ins--;
    }
    
    printf("Ans = %d\n", share_memory->sum);
    return 0;
}
```





## 2.5条件变量cond

1. 如何给多个进程发信号？

   就得用到cond！！

2. cond底层就已经包含了signal机制！



![img](https://wx4.sinaimg.cn/mw690/005LasY6gy1gdl04ytddnj318c0jinkb.jpg)



### 相关函数(pthread_cond系列)

和pthread_mutex系列极其相似！



#### cond_init

![img](https://wx3.sinaimg.cn/mw690/005LasY6gy1gdl08j8ao4j31ni0o6dvw.jpg)

![img](https://wx4.sinaimg.cn/mw690/005LasY6gy1gdl0k8kn6zj31n50u0hdt.jpg)







SYNOPSIS

1. `pthread_cond_signal(cond)`就是给cond条件变发送信号！
2. 由`cond_wait()`参数可知，cond一定得配合mutex使用！



DESCRIPTION

1. 第一段的第二句话就是概述！

2. 第二段讲述了为什么要配合mutex使用！！

   因为线程之间是共享资源的，不能让多个线程同时访问到cond！

3. 第三段讲到了其实在linux中`cond_attr`是被忽略的，所以写null就可！

4. 5、6段讲了signal和broadcast的区别！

5. ==7、8段是核心！==

   + 用`cond_wait()`之前必须先加锁！
   + 然后`cond_wait()`会自动解锁(==方便别的线程访问资源！==)，并将此线程挂起，等待signal
   + 接收到signal之后，会自动加锁！！
   + 第八段没看懂。。。





### cond应用--C/S架构的信息传递



![img](https://wx3.sinaimg.cn/mw690/005LasY6gy1gdl2w46zuaj31400u0ty2.jpg)

1. 上面是server端的逻辑，下面是client端的逻辑

2. 这就很好地体现了`cond_wait()`为什么要先unlock再挂起！

   --因为为了释放这块共享内存！





#### Server端



```c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <pthread.h>

struct Shm {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
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
    
    printf("After shmget!\n");
    
    if ((share_memory = (struct Shm *)shmat(shmid, 0, 0)) <= 0) {
        perror("shmat");
        return 1;
    }
    
    printf("After shmat!\n");
    
    memset(share_memory, 0, sizeof(struct Shm));
    
    //下面是核心内容！！！！！！！！！！！！！！！！！！！！！
    pthread_mutexattr_t m_attr;
    pthread_condattr_t c_attr;//cond属性变量的声明
    
    pthread_mutexattr_init(&m_attr);
    pthread_condattr_init(&c_attr);//属性初始化
    
    pthread_mutexattr_setpshared(&m_attr, PTHREAD_PROCESS_SHARED);
    pthread_condattr_setpshared(&c_attr, PTHREAD_PROCESS_SHARED);//设置进程间共享
    
    pthread_mutex_init(&share_memory->mutex, &m_attr);
    pthread_cond_init(&share_memory->cond, &c_attr);//cond初始化
    
    
    
    while (1) {
        pthread_mutex_lock(&share_memory->mutex);
        
        printf("After mutex lock!\n");
        pthread_cond_wait(&share_memory->cond, &share_memory->mutex);//等待信号！
        //如果收到信号后没抢到锁，会咋样？？会忽略这次的信号吗？宿哥说是会(然后就继续进行下去？？？到底会不会继续呢 还是去看man手册把！！)，所以为了让server端抢到mutex，就在client中加了usleep(1000)！！！！！！
        
        
        printf("After cond wait!\n");
        printf("<Recv> : %s\n", share_memory->buff);
        sleep(2);//请思考在这加sleep碍不碍事？逻辑有没有问题？(肯定是没问题的，但是要想清楚为啥)，而且加上了更好！
        
        pthread_mutex_unlock(&share_memory->mutex);
    }
    return 0;
}
```



PS: 

1. ==还是很不懂为啥cond_wait()非得绑定个mutex，还非得在用之前锁上，用之后解锁？这不是多此一举么？？？？==

2. 后来我思考了很久，可能是因为

   在上一段代码的57-58行之间别的线程无法访问cond以及共享内存，为此而设计的！！

#### client端



```c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <pthread.h>

struct Shm {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
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
    
    //为什么不在这初始化锁和cond呢？因为只需要server端初始化就够了！！
    
    while (1) {
        printf("Before mutex lock!\n")
        pthread_mutex_lock(&share_memory->mutex);//锁上！
        printf("After mutex lock!\n");
  
        printf("<client> : ");
        scanf("%[^\n]s", share_memory->buff);
        getchar();//吞回车
  		
        /*下面是宿哥视频里讲的，但是我感觉和视频后面他讲的矛盾！！
        就是后面他讲，如果server端没抢到mutex，会忽略这次信号！！而不是等解锁后继续！
        
        pthread_cond_signal(&share_memory->cond);//发信号！
        sleep(1);//必须是先发信号，然后server端接收到信号，尝试着给mutex加锁，但是由于这边还没解锁所以会阻塞！！等这边解锁之后那边才继续running！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！
        */
        
        pthread_mutex_unlock(&share_memory->mutex);//解锁
        pthread_cond_signal(&share_memory->cond);//发信号！
       	usleep(1000);//停留极短的时间！！
    }
    return 0;
}
```



==还是对各种加锁解锁有点懵。。。。==









# 3.PIPE

