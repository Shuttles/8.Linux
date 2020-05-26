/*************************************************************************
	> File Name: server.c
	> Author: 
	> Mail: 
	> Created Time: 二  5/26 17:11:49 2020
 ************************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#define MAXBUF 256
int main(int argc, char const *argv[])
{
	int s = 0;
	int n = 0;
	int reuse = 1;
	int cli_len = sizeof(struct sockaddr);
	int port = 1987;
	char buf[MAXBUF] = {0};
	struct sockaddr_in addr, cli;

	/*初始化本地监听端口信息*/
	bzero(&addr, sizeof(addr));
	addr.sin_family = PF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);

	/*创建UDP套节字*/
	s = socket(AF_INET, SOCK_DGRAM, 0);
	if (s<0)
	{
		perror("socket");
		return -1;
	}

	/*允许端口复用*/
	setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

	/*绑定指定端口*/
	if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0)
	{
		perror("bind");
		return -1;
	}

	while(1){
		memset(buf, 0, MAXBUF);
		/*从套节字s中读取数据*/
		n = recvfrom(s, buf, MAXBUF, 0, (struct sockaddr *)&cli, &cli_len);
		if(n<0){
			perror("recvfrom");
			return -1;
		}else{
			printf("receive msg from %s(port=%d) len %d: %s\n",inet_ntoa(cli.sin_addr), port, n, buf);
		}
	}

	return 0;
}
