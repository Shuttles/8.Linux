/*************************************************************************
	> File Name: popen.czy.c
	> Author: 少年宇
	> Mail: 
	> Created Time: 2020年04月09日 星期四 14时50分46秒
 ************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_SIZE 1024
#define SHELL "/bin/bash"

FILE *my_popen(const char *command, const char *type) {
    if ((type[0] != 'r' && type[0] != 'w') || type[1] != 0) {
        fprintf(stderr, "Usage : command type \n");
        return NULL;
    }
    int pipefd[2];
    pid_t pid;
    char buff[MAX_SIZE];

    if (pipe(pipefd) < 0) {
        fprintf(stderr, "my_popen() pipe create error!\n");
        return NULL;
    }
    
    if ((pid = fork()) < 0) {
        perror("fork");
        return NULL;
    }

    if (pid == 0) {
        if (type[0] == 'r') {
            close(pipefd[0]);//在资金从中应该将结果写入管道，这样父进程返回的文件指针才能读，所以这里将读端关闭
            dup2(pipefd[1], STDOUT_FILENO);
            close(pipefd[1]);
        } else {
            close(pipefd[1]);
            dup2(pipefd[0], STDIN_FILENO);
            close(pipefd[0]);
        }

        //exec
        if (execlp(command, command, NULL) < 0) {
            return NULL;
        }

        exit(0);   
    } else {
        wait(NULL);
        if (type[0] == 'r') {
            close(pipefd[1]);
            FILE *fp = NULL;
            if ((fp = fdopen(pipefd[0], "r")) == NULL) {
                perror("fdopen");
                return NULL;
            }
            return fp;
        } else {
            FILE *fp = NULL;
            if ((fp = fdopen(pipefd[1], "w")) == NULL) {
                perror("fdopen");
                return NULL;
            }
        }
    }
    return NULL;
}
