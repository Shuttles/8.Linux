/*************************************************************************
	> File Name: shm1.c
	> Author: 少年宇
	> Mail: 
	> Created Time: 2020年04月04日 星期六 19时10分17秒
 ************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

struct SHM {
    pid_t s_pid;
    char buff[512];
    pthread_mutex_t mutex;
    pthread_cond_t cond;
};

struct SHM *share_memory = NULL;


int main() {
    pid_t pid;
    int shmid;
    key_t key = ftok(".", 198);

    if ((shmid = shmget(key, sizeof(struct SHM), IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        return 1;
    }

    if ((share_memory = shmat(shmid, NULL, 0)) == NULL) {
        perror("shmat");
        return 1;
    }
    
    //share_memory->s_pid = getpid();

    //memset(share_memory, 0, sizeof(struct SHM));


    pthread_mutexattr_t m_attr;
    pthread_condattr_t c_attr;

    pthread_mutexattr_init(&m_attr);
    pthread_condattr_init(&c_attr);

    pthread_mutexattr_setpshared(&m_attr, PTHREAD_PROCESS_SHARED);

    pthread_mutex_init(&share_memory->mutex, &m_attr);
    pthread_cond_init(&share_memory->cond, &c_attr);//第二个参数可以被忽略？
    

    while (1) {
        //不能用signal了！
        //signal(SIGUSR2, print);
        pthread_mutex_lock(&share_memory->mutex);

        printf("After mutex_lock!\n");
        pthread_cond_wait(&share_memory->cond, &share_memory->mutex);

        printf("After cond wait!\n");
        printf("%s\n", share_memory->buff);

        pthread_mutex_unlock(&share_memory->mutex);
    }

    return 0;
}
