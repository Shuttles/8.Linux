/*************************************************************************
	> File Name: sum.c
	> Author: 少年宇
	> Mail: 
	> Created Time: 2020年04月05日 星期日 15时17分47秒
 ************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/shm.h>
#include <sys/wait.h>

struct Shm {
    int flag;
    int now;
    int sum;
};

struct Shm *share_memory = NULL;

void child_do(int max) {
    while (1) {
        while (share_memory->flag) {

        }

        share_memory->flag = 1;

        if (share_memory->now > max) break;

        share_memory->sum += share_memory->now;
        share_memory->now++;

        share_memory->flag = 0;
    }

    return;
}


int main(int argc, char **argv) {
    pid_t pid;
    int shmid;
    key_t key = ftok(".", 198);

    if ((shmid = shmget(key, sizeof(struct Shm), IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        return 1;
    }

    if ((share_memory = shmat(shmid, 0, 0)) <= 0) {
        perror("shmat");
        return 1;
    }
    

    int ins = atoi(argv[2]);
    for (int i = 0; i < ins; i++) {
        if ((pid = fork()) < 0) {
            perror("fork");
            continue;
        }    

        if (pid == 0) {
            int max = atoi(argv[1]);
            child_do(max);
            exit(0);
        }
    }

    while (ins) {
        wait(NULL);
        ins--;
    }
    
    printf("share_memory->now = %d, share_memory->sum = %d\n", share_memory->now, share_memory->sum);
    return 0;
}
