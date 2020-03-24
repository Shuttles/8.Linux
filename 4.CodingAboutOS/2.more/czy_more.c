/*************************************************************************
	> File Name: su_more.c
	> Author: 少年宇
	> Mail: 
	> Created Time: 2020年03月21日 星期六 19时29分16秒
 ************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define PAGELINE 15
#define LENLINE 512

void do_more(FILE *fp);
int get_cmd();

int main(int argc, char *argv[]) {
    FILE *fp;
    if (argc == 1) printf("Usage: %s [filename | stdin]\n", argv[0]);
    else 
        while (--argc) {
            if ((fp = fopen(*++argv, "r")) != NULL) {
                do_more(fp);
            }
        }

    return 0;
}

void do_more(FILE *fp) {
    char line[LENLINE] = {0};
    int num_line = 0, reply;//get_cmd();
    while (fgets(line, LENLINE, fp)) {
        if (num_line == PAGELINE) {
            reply = get_cmd();
            switch (reply) {
                case 0: break;
                case 1: {
                    
                } break; 
                case 2: {
                    if (fputs(line, stdout) == EOF) {
                        perror("fupts");
                        exit(1);
                    }
                    continue;        
                } break;
                default: {
                    //如果输入的不是空格、q、回车
                }
            }
        }
        if (fputs(line, stdout) == EOF) {
            perror("fputs");
            exit(1);
        }
        num_line++;
    }
} 

int get_cmd() {
    printf("more:");
    int c;
    while ((c = getchar()) != EOF) {
        if (c == 'q') return 0;
        if (c == ' ') return 1;
        if (c == '\n') return 2;
    }
    return -1;
}
