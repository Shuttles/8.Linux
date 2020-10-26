
#include <curses.h>

void createMap() {
    initscr();
    clear();
    move(0, 0);
    for (int i = 0; i < 22; i++) addstr("-----");
   addstr(">");
    for (int i = 1; i < 40; i++) {
        move(i, 0);
        addstr("|");
    }
    move(40, 0);
    addstr("V");
    move(3, 3);
    for (int i = 0; i < 21; i++) addstr("-----");
    for (int i = 4; i < 35; i++) {
        move(i, 3);
        addstr("|");
    }
    move(35, 3);
    for (int i = 0; i < 21; i++) addstr("-----");
    for (int i = 4; i < 35; i++) {
        move(i, 107);
        addstr("|");
    }
    for (int i = 4; i < 35; i++) {
        move(i, 55);
        addstr("|");
    }
    for (int i = 16; i < 23; i++) {
        move(i, 5);
        addstr("|");
    }
    for (int i = 16; i < 23; i++) {
        move(i, 105);
        addstr("|");
    }
    move(16, 4);
    addch('-');
    move(22, 4);
    addch('-');
    move(16, 106);
    addstr("-");
    move(22, 106);
    addstr("-");
    refresh();
    getchar();
    endwin();
}

int main() {
    createMap();
    return 0;
}
