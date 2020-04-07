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
    

    /*
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);

    pthread_mutex_init(&share_memory->mutex, &attr);

    pthread_cond_init(&share_memory->cond, NULL);//第二个参数可以被忽略？
    
    */

    while (1) {
        pthread_mutex_lock(&share_memory->mutex);
    
        printf("<child> : ");
        scanf("%[^\n]s", share_memory->buff);
        getchar();

        pthread_mutex_unlock(&share_memory->mutex);
        pthread_cond_signal(&share_memory->cond);
        printf("send signal!\n");
        //不能用signal了！！
        // kill(share_memory->s_pid, SIGUSR2);
    }
    return 0;
}
