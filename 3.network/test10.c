/*************************************************************************
	> File Name: test10.c
	> Author: 少年宇
	> Mail: 
	> Created Time: 2020年03月24日 星期二 18时57分28秒
 ************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

int main() {
    #define MAX_N 10
    for (int i = 0; i < MAX_N; i++) {
        int pid;
        if ((pid = fork()) < 0) {
            perror("fork");
            exit(1);
        }

        if (pid == 0) {
            sleep(60);
            exit(0);
        }
    }
    sleep(60);
    return 0;
}

