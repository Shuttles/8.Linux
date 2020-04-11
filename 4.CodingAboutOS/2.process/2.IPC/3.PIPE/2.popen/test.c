/*************************************************************************
	> File Name: test.c
	> Author: 少年宇
	> Mail: 
	> Created Time: 2020年04月09日 星期四 14时40分54秒
 ************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_SIZE 4
#define SHELL "/bin/sh"

static pid_t *childpid = NULL;//指向运行时分配的用于存储子进程pid的数组
FILE *my_popen(const char *command, const char *type);
int my_pclose(FILE *fp);

int main(){
    FILE *fp1 = NULL;
    FILE *fp2 = NULL;
    char buff[1024] = {0};
    fp1 = my_popen("cat","w");
    fp2 = fopen("a.log","r");
    fread(buff,1,1024,fp2);
    fwrite(buff,1,1024,fp1);
    my_pclose(fp1);
    fclose(fp2);
    return 0;
}

FILE *my_popen(const char *command, const char *type) {
    if ((type[0] != 'r' && type[0] != 'w') || type[1] != 0) {
        fprintf(stderr, "Usage : command type \n");
        return NULL;
    }

    //如果是第一次调用my_popen
    if (childpid == NULL) {
        childpid = calloc(MAX_SIZE, sizeof(pid_t));
    }


    int pipefd[2];
    pid_t pid;
    FILE *fp = NULL;

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
            //这时type[0] == 'w';
            close(pipefd[1]);
            dup2(pipefd[0], STDIN_FILENO);
            close(pipefd[0]);
        }

        //将子进程中所有的fd对应的文件流都关闭
        for (int i = 0; i < MAX_SIZE; i++) {
            if (childpid[i]) 
                close(i);
        }

        //exec
        if (execl(SHELL, "sh", "-c", command,  NULL) < 0) {
            return NULL;
        }

        exit(0);
    } else {
        //wait(NULL);不需要wait！！！！
        if (type[0] == 'r') {
            close(pipefd[1]);
            if ((fp = fdopen(pipefd[0], "r")) == NULL) {
                perror("fdopen");
                return NULL;
            }
        } else {
            close(pipefd[0]);
            if ((fp = fdopen(pipefd[1], "w")) == NULL) {
                perror("fdopen");
                return NULL;
            }
        }
    }
    childpid[fileno(fp)] = pid;//remember child pid for this fd;
    return fp;
}



int my_pclose(FILE *fp) {
    int fd, stat;
    pid_t pid;

    if (childpid == NULL) {
        return -1;//popen has never been called;
    }

    fd = fileno(fp);
    if ((pid = childpid[fd]) == 0) {
        return -1;//fp wasn't opened by popen();
    }

    childpid[fd] = 0;
    if (fclose(fp) == EOF) {
        return -1;
    }

    while (waitpid(pid, &stat, 0) < 0) {

    }

    return stat;//return child's termination status;
}

