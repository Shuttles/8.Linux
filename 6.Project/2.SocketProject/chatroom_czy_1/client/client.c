/*************************************************************************
	> File Name: client.c
	> Author: 少年宇
	> Mail: 
	> Created Time: 2021年03月17日 星期三 14时19分37秒
 ************************************************************************/

#include <head.h>
#include <tcp_client.h>

int main(int argc, char *argv[]) {
    int sockfd;
    if (argc != 3) {
        fprintf(stderr, "Usage : %s ip port\n", argv[0]);
        return 1;
    }

    if ((sockfd = socket_connect(argv[1], atoi(argv[2]))) < 0) {
        perror("socket_connect");
        return 2;
    }



    return 0;
}
