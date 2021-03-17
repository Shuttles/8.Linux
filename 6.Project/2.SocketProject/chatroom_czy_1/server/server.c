/*************************************************************************
	> File Name: server.c
	> Author: 少年宇
	> Mail: 
	> Created Time: 2021年03月17日 星期三 14时11分16秒
 ************************************************************************/

#include <head.h>
#include <tcp_server.h>

int main(int argc, char *argv[]) {
    int port, server_listen;
    if (argc != 2) {
        fprintf(stderr, "Usage : %s port\n", argv[0]);
        return 1;
    }

    port = atoi(argv[1]);

    if ((server_listen = socket_create(port)) < 0) {
        perror("socket_create");
        return 2;
    }




    return 0;
}
