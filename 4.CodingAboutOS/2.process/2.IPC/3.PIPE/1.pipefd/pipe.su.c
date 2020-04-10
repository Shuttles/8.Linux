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
    pipe(pipefd);//这个函数作用是啥？？
    char buff[1024] = {0};
    if ((pid = fork()) < 0) {
        perror("fork");
        return 1;
    }

    //fp = fdopen(pipefd[0], "r");
    //fp1 = fdopen(pipefd[1], "w");
    if (pid == 0) {
        while (1) {
            scanf("%[^\n]s", buff);
            getchar();
            //fprintf(fp1, "%s", buff);
            write(pipefd[1], buff, strlen(buff));
        }
    } else {
        while (1) {
            memset(buff, 0, sizeof(buff));
            read(pipefd[0], buff, 1024);
            printf("server: %s\n", buff);
        }
    }

    return 0;
}
