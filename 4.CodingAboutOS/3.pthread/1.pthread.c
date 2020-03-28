/*************************************************************************
	> File Name: 1.pthread.c
	> Author: 少年宇
	> Mail: 
	> Created Time: 2020年03月26日 星期四 19时50分13秒
 ************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

struct Msg {
    int age;
    char name[20];
};

void *print(void *arg) {
    struct Msg *temp;
    temp = (struct Msg *)arg;
    printf("name = %s, age = %d\n", temp->name, temp->age);
    int retval = 3;
    pthread_exit((void *)&retval);
    return NULL ;
}



int main() {
    void *status;
    pthread_t tid;
    struct Msg msg;
    msg.age = 18;
    strcpy(msg.name, "chenzheyu");
    pthread_create(&tid, NULL, print, (void *)&msg);
    sleep(1);
    pthread_join(tid, &status);
    printf("%d\n", *(int *)(status));
    return 0;
}
