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



int main() {
    pid_t pid;
    int pipefd[2];
    pipe(pipefd);
    if ((pid = fork()) < 0) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        while (1) {
            fscanf(pipefd[1], "%[^\n]s", );
            getchar();
        }
    } else {
        while (1) {
            f
        }
    }

    return 0;
}
