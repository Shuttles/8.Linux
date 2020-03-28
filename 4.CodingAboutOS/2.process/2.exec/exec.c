/*************************************************************************
	> File Name: 1.cpp
	> Author: 少年宇
	> Mail: 
	> Created Time: 2020年03月26日 星期四 16时43分23秒
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>


#define NAMEMAX 256

int main(int argc, char **argv) {
    int  pid;
    char filename[NAMEMAX] = {0};

    if (argc != 2) {
        fprintf(stderr, "Usage %s codefile\n", argv[0]);
        exit(1);
    }

    strcpy(filename, argv[1]);
    if ((pid = fork()) < 0) {
        perror("fork");
        exit(1);
    }
    if (pid == 0) {
        //第一个子进程，用vim打开文件
        execlp("vim", "vim", filename, NULL);
        exit(0);
    } else {
        wait(NULL);//使父进程阻塞
        if ((pid = fork()) < 0) {
            perror("fork");
            exit(1);
        } 
        if (pid == 0) {
            //孙子进程，编译文件
            execlp("gcc", "gcc", "-o", "test", filename, NULL);
            exit(0);
        } else {
            wait(NULL);
            if ((pid = fork()) < 0) {
                perror("fork");
                exit(1);
            } 
            if (pid == 0) {
                //重孙进程,运行文件
                execlp("./test", "./test", NULL);
                exit(0);
            } else {
                wait(NULL);
            }
        }
    }
    return 0;
}
