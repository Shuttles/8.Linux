/*************************************************************************
	> File Name: su_exec.c
	> Author: 少年宇
	> Mail: 
	> Created Time: 2020年03月26日 星期四 18时14分05秒
 ************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
int main(int argc, char **argv) {
    pid_t pid;
    char filename[512] = {0};
    char o_name[512] = {0};//编译之后的name
    char f_type[256] = {0};//文件类型
    char cmd[512] = {0};//编译指令
    if (argc < 2) {
        fprintf(stderr, "Usage : %s filename arg...\n", argv[0]);
        return 1;
    }

    strcpy(filename, argv[1]);

    //a.c b.cpp
    char *sub;
    sub = strrchr(filename, '.');
    strncpy(o_name, filename, sub - filename);
    strcpy(f_type, sub);

    if (!strcmp(f_type, ".c")) {
        strcpy(cmd, "gcc");
    } else if (!strcmp(f_type, ".cpp")) {
        strcpy(cmd, "g++");
    } else {
        fprintf(stderr, "Not support File Type!\n");
        return 2;
    }

    if ((pid = fork()) < 0) {
        perror("fork");
        return 3;
    }

    if (pid == 0) {
        execlp("vim", "vim", filename, NULL);
    }

    wait(NULL);
    
    if ((pid = fork()) < 0) {
        perror("fork");
        return 3;
    }
    if (pid == 0) {
        execlp(cmd, cmd, filename, "-o", o_name, NULL);//gcc filename -o o_name
    }

    int status;
    wait(&status);
    if (status == 0) {
        printf("OK\n");
        char name[256] = {0};
        sprintf(name, "./%s", o_name);
        execlp(name, name, NULL);//      ./a.out
    } else {
        printf("Failed!\n");
        return 4;
    } 
    
    return 0;
}
