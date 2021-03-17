/*************************************************************************
	> File Name: common.c
	> Author: 少年宇
	> Mail: 
	> Created Time: 2021年03月17日 星期三 15时47分23秒
 ************************************************************************/

#include "head.h"

char *get_value(char *path, char *key) {
    FILE *fp = NULL;
    ssize_t nread;
    char *line = NULL, *sub = NULL;
    extern char conf_ans[50];
    size_t linecap;
    if (path == NULL || key == NULL) {
        fprintf(stderr, "Error in argument!\n");
        return NULL;
    }
    if ((fp = fopen(path, "r")) == NULL) {
        perror("fopen");
        return NULL;
    }
    
    while ((nread = getline(&line, &linecap, fp)) != -1) {
        if ((sub = strstr(line, key)) == NULL) continue;
        else {
            if (line[strlen(key)] == '=') {
                strncpy(conf_ans, sub + strlen(key) + 1, nread - strlen(key) - 2);
                *(conf_ans + nread - strlen(key) - 2) = '\0';
                break;
            }
        }
    }
    free(line);
    fclose(fp);
    return (sub == NULL ? NULL : conf_ans);
}


void make_nonblock_ioctl(int fd) {
    unsigned long ul = 1;
    ioctl(fd, FIONBIO, &ul);
}

void make_block_ioctl(int fd) {
    unsigned long ul = 0;
    ioctl(fd, FIONBIO, &ul);
}

void make_nonblock(int fd) {
    int flag;
    if ((flag = fcntl(fd, F_GETFL)) < 0) {
        return ;
    }
    flag |= O_NONBLOCK;
    fcntl(fd, F_SETFL, flag);
}

void make_block(int fd) {
    int flag;
    if ((flag = fcntl(fd, F_GETFL) < 0)) {
        return ;
    }
    flag &= ~O_NONBLOCK;
    fcntl(fd, F_SETFL, flag);
}

