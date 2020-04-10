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
    FILE *fp, *fp1;
    int pipefd[2];
    if (pipe(pipefd) < 0) {
        perror("pipe");
        return 1;
    }
    char buff[1024] = {0};
    if ((pid = fork()) < 0) {
        perror("fork");
        return 1;
    }

    fp = fdopen(pipefd[0], "r");
    fp1 = fdopen(pipefd[1], "w");
    if (pid == 0) {
        while (1) {
            scanf("%[^\n]s", buff);
            getchar();
            fprintf(fp1, "%s\n", buff);//这里在格式化字符串中加了\n,这是因为fscanf的原因
            fflush(fp1);//这里加了一行
        }
    } else {
        while (1) {
            fscanf(fp, "%s", buff);
            //getchar();
            printf("server: %s\n", buff);
        }
    }

    return 0;
}
