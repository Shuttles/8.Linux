/*************************************************************************
	> File Name: 1.select.c
	> Author: 少年宇
	> Mail: 
	> Created Time: 2021年10月08日 星期五 19时26分26秒
 ************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
    fd_set rfds;
    struct timeval tv;
    int retval;

    /* Watch stdin(fd 0) to see when it has input.*/

    FD_ZERO(&rfds);
    FD_SET(0, &rfds);

    /* Wait up to five seconds. */

    tv.tv_sec = 5;
    tv.tv_usec = 0;

    retval = select(1, &rfds, NULL, NULL, &tv);
    /* Don't rely on the value of tv now!*/
    /* 不是很懂上面那句话什么意思*/
    
    if (retval == -1) {
        perror("select.");
    } else if (retval) {
        printf("Data is available now.\n");
        /* FD_ISSET(0, &rfds) will be true.*/
    
        char buffer[512] = {0};
        scanf("%s", buffer);
        printf("Data = %s\n", buffer);
    } else {
        printf("No data within five seconds.\n");
    }

    return 0;
}

