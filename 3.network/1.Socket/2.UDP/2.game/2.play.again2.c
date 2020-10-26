/*************************************************************************
	> File Name: 1.play_again1.c
	> Author: 少年宇
	> Mail: 
	> Created Time: 2020年05月28日 星期四 20时00分26秒
 ************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/select.h>
#include <string.h>
#include <ctype.h>

#define TIMEOUT 5
#define Q "Do you want another try?\n"

int get_response();
void set_cr_noecho_mode();
void tty_mode(int how);
void set_nonblock_mode();
int get_ok_char();
int get_response();


int main() {
    int response;
    tty_mode(0);
    set_cr_noecho_mode();

    response = get_response();
    tty_mode(1);
    printf("\nreturn val = %d\n", response);
    return response;
}

int get_response() {
    printf("%s (y/n)?", Q);
    fflush(stdout);

    struct timeval tv;
    tv.tv_sec = TIMEOUT;
    tv.tv_usec = 0;

    fd_set set;
    FD_ZERO(&set);
    FD_SET(0, &set);
    
    int ret = select(1, &set, NULL, NULL, &tv);
    if (ret == 1) {
        if (FD_ISSET(0, &set)) {
            //意味着可读
            while (1) {
                int input = tolower(getchar());
                switch (input) {
                    case 'y':
                        return 1;
                    case 'n':
                        return 0;

                }
            }
        }
    }
    return 2;
}

int get_ok_char() {
    int c;
    while ((c = getchar()) != EOF && strchr("yYnN", c) == NULL) ;
    return c;
}


/*
int get_response() {
    printf("%s", Q);
    printf("(y/n)");
    switch (getchar()) {
        case 'y':
        case 'Y': return 1;
        case 'N':
        case 'n': return 0;
        default: ;
    }
}
*/

void set_nonblock_mode() {
    int termflags;
    termflags = fcntl(0, F_GETFL);
    termflags |= O_NONBLOCK;
    fcntl(0, F_SETFL, termflags);
    

}

void set_cr_noecho_mode() {
    struct termios ttystate;
    tcgetattr(0, &ttystate);
    ttystate.c_lflag &= ~ICANON;
    ttystate.c_lflag &= ~ECHO;
    ttystate.c_cc[VMIN] = 1;
    ttystate.c_cc[VTIME] = 0.0005;
    tcsetattr(0, TCSANOW, &ttystate);

}

void tty_mode(int how) {//how = 1;还原 how = 0保存
    static struct termios orignal_mode;
    if (how == 0) {
        tcgetattr(0, &orignal_mode);
    } else {
        tcsetattr(0, TCSANOW, &orignal_mode);
    }
    return ;
}
