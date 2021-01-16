/*************************************************************************
	> File Name: test.c
	> Author: 少年宇
	> Mail: 
	> Created Time: 2021年01月16日 星期六 18时17分45秒
 ************************************************************************/

#include "../common/head.h"
#include "../common/common.h"

char ans[50] = {0};

int main() {
    get_value("./a.conf", "SERVERIP");
    printf("%s\n", ans);
    return 0;
}
