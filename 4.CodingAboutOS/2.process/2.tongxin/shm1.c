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


char *share_memory = NULL;

void print(int sig) {
    printf("<parent> : %s\n", share_memory);
}

int main() {
    pid_t pid;
    int shmid;
    key_t key = ftok(".", 198);

    if ((shmid = shmget(key, 4096, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        return 1;
    }

    if ((share_memory = shmat(shmid, NULL, 0)) < 0) {
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
            kill(getppid(), SIGUSR2);
        }
    } else {
        while (1) {
            signal(SIGUSR2, print);
        }
    }

    return 0;
}
