/*************************************************************************
	> File Name: common.h
	> Author: 少年宇
	> Mail: 
	> Created Time: 2021年01月16日 星期六 17时57分20秒
 ************************************************************************/

#ifndef _COMMON_H
#define _COMMON_H

char conf_ans[50] = {0};
char *get_value(char *path, char *key);//获取配置信息
void make_nonblock_ioctl(int fd);
void make_block_ioctl(int fd);
void make_nonblock(int fd);
void make_block(int fd);

#endif
