/*************************************************************************
	> File Name: 1.c
	> Author: 少年宇
	> Mail: 
	> Created Time: 2020年05月30日 星期六 20时33分10秒
 ************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ncurses.h>

int main() {
    initscr();
    /*正式内容开始*/
    for (int i = 1; i <= 105; i++) printw("-");
    printw(">\n");

    printw("|");
    for (int i = 1; i <= 100; i++) printw("-");
    printw("\n");

    for (int i = 1; i <= 35; i++) {
        for (int j = 1; j <= 100; j++) {
            if (j == 1 || j == 2 || j == 100)  printw("|");
            else printw(" ");
        }
        printw("\n");
    }
    

    


    /*正式内容结束*/
    refresh();
    getchar();
    endwin();
    return 0;
}
