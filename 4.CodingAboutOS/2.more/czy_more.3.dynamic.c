/*************************************************************************
	> File Name: su_more.c
	> Author: 少年宇
	> Mail: 
	> Created Time: 2020年03月21日 星期六 19时29分16秒
 ************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>

//#define PAGELINE 15
#define LENLINE 512

int PAGELINE = 0;

void do_more(FILE *fp);
int get_cmd(FILE *fp);

int main(int argc, char *argv[]) {
    FILE *fp;
    if (argc == 1) //printf("Usage: %s [filename | stdin]\n", argv[0]);
        do_more(stdin);
    else 
        while (--argc) {
            if ((fp = fopen(*++argv, "r")) != NULL) {
                do_more(fp);
            }
        }

    return 0;
}

void do_more(FILE *fp) {
    struct winsize size;
    ioctl(STDIN_FILENO, TIOCGWINSZ, &size);
    PAGELINE  = (size.ws_row - 1);
    char line[LENLINE] = {0};
    FILE *cmd = fopen("/dev/tty", "r");
    int num_line = 0, reply;//get_cmd();
    while (fgets(line, LENLINE, fp)) {
        if (num_line == PAGELINE) {
            reply = get_cmd(cmd);
            if (reply == 0) break;
            num_line -= reply;//妙啊！！！

        }
        //输出
        if (fputs(line, stdout) == EOF) {
            perror("fputs");
            exit(1);
        }
        num_line++;
    }
} 

int get_cmd(FILE *fp) {
    printf("more:");
    int c;
    while ((c = fgetc(fp)) != EOF) {
        if (c == 'q') return 0;
        if (c == ' ') return PAGELINE;
        if (c == '\n') return 1;
    }
    return -1;
}
