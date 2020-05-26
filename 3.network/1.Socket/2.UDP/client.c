/*************************************************************************
	> File Name: client.c
	> Author: 
	> Mail: 
	> Created Time: 二  5/26 14:11:17 2020
 ************************************************************************/

#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<string.h>
#include<stdlib.h>
int main() {
    int sockfd = socket(AF_INET,SOCK_DGRAM,0);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8888);
    addr.sin_addr.s_addr = INADDR_ANY;

    char *buf= (char *)malloc(sizeof(char) * 1024);
    while(1) {
        memset(buf,0,sizeof(buf));
        scanf("%s",buf);
        sendto(sockfd,buf,sizeof(buf),0,(struct sockaddr*)&addr,sizeof(addr));
/*
        socklen_t len=sizeof(addr);
        recvfrom(sockfd,buf,sizeof(buf),0,(struct sockaddr*)&addr,&len);
        printf("%s",buf);
*/
    }
    close(sockfd);
    return 0;
}
