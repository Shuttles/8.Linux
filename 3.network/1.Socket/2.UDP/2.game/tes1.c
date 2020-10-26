/*************************************************************************
	> File Name: tes1.c
	> Author: 少年宇
	> Mail: 
	> Created Time: 2020年05月30日 星期六 19时30分39秒
 ************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <curses.h>

int main() {
    initscr();
    clear();
    move(10, 20);
    addstr("Hello Haizei!");
    refresh();

    getchar();
    //gcc tes1.c -lncurses
    endwin();
    return 0;
}
