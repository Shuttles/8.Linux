/*************************************************************************
	> File Name: server.c
	> Author: 
	> Mail: 
	> Created Time: 二  5/26 10:11:49 2020
 ************************************************************************/

#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<string.h>
int main() {
    int sockfd = socket(AF_INET,SOCK_DGRAM,0);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8888);
    addr.sin_addr.s_addr = INADDR_ANY;
    int ret = bind(sockfd,(struct sockaddr*)&addr,sizeof(addr));

    if(ret < 0) {
        perror("bind");
        exit(1);

    }

    struct sockaddr_in cli;
    socklen_t len=sizeof(cli);
    char * buf = (char *) malloc(sizeof(char) * 1024);
    while(1)
    {
        memset(buf,0,sizeof(buf));
        recvfrom(sockfd,buf,sizeof(buf),0,(struct sockaddr*)&cli,&len);
        printf("recv =%s\n",buf);
    /*
        memset(buf,0,sizeof(buf));
        buf ="have recved";
        sendto(sockfd,buf,sizeof(buf),0,(struct sockaddr*)&cli,len);
    */
    }
    close(sockfd);
    return 0;
}
