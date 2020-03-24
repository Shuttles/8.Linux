/*************************************************************************
	> File Name: su_ls.c
	> Author: 少年宇
	> Mail: 
	> Created Time: 2020年03月19日 星期四 18时58分29秒
 ************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <math.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

#define FILEMAX 1024
#define NAMEMAX 256

int flag_a = 0, flag_l = 0;
int dir_num = 0;//防止只有一个目录时不需要：

int fg_c = 0, bg_c = 0;

void update_color(mode_t mode);

void do_ls(char *dirname);
void do_stat(char *filename);
int cmp_name(const void *_a, const void *_b) {
    char *a = (char *)_a;
    char *b = (char *)_b;
    return strcmp(a, b);
}
void size_window(char names[][NAMEMAX], int cnt, int *row, int *col);
void show_files(char filename[][NAMEMAX], int cnt, int row, int col);
void show_info(char *filename, struct stat *info);
char *uid_to_name(uid_t uid);
char *gid_to_name(gid_t gid);

void print_in_color_l(char *filename, struct stat *info);
void print_in_color(char *filename, struct stat *info, int arg);

int main(int argc, char *argv[]) {
    int opt;
    while ((opt = getopt(argc, argv, "al")) != -1) {
        switch (opt) {
            case 'a': flag_a = 1;break;
            case 'l': flag_l = 1;break;
            default: 
                fprintf(stderr, "Usage: %s [-al] [filename]\n", argv[0]);
                exit(1);
        }
    }

    printf("flag_a = %d, flag_l = %d\n", flag_a, flag_l);

    printf("optind = %d\n", optind);

    argc -= (optind - 1);
    argv += (optind - 1);

    printf("argc = %d, argv = %s\n", argc, *(argv + 1));

    if (argc == 1) {
        dir_num = 0;
        do_ls(".");
    } else {
        dir_num = argc - 2;
        while (--argc) {
            do_ls(*(++argv));
        }
    }
    return 0;
}

void do_ls(char *dirname) {
    DIR *dirp = NULL;
    struct dirent *direntp = NULL;
    char names[FILEMAX][NAMEMAX] = {0};
    if ((dirp = opendir(dirname)) == NULL) {
        if (access(dirname, R_OK) == 0) {
            if (flag_l == 0) {
                dir_num--;
                struct stat tmp_st;
                lstat(dirname, &tmp_st);
                update_color(tmp_st.st_mode);
                printf("\033[%d;%dm%s\033[0m\n", bg_c, fg_c, dirname);
                return ;
            } else {
                dir_num--;
                do_stat(dirname);
                return;
            }
        } else {
            perror(dirname);
            return;
        }
    } else { //如果是目录
        if (dir_num)
            printf("%s:\n", dirname);
        chdir(dirname);//切换到那个目录
        int cnt = 0;
        while ((direntp = readdir(dirp)) != NULL) {
            if ((flag_a == 0) && direntp->d_name[0] == '.') continue;
            strcpy(names[cnt++], direntp->d_name);
        }
        qsort(names, cnt, NAMEMAX, cmp_name);

        if (flag_l == 1) {//如果有-l
            for (int i = 0; i < cnt; i++) {
                do_stat(names[i]);
            }
        } else {//如果没有-l
            //printf("Print all files\n");
            int row, col;
            size_window(names, cnt, &row, &col);
            //printf("row = %d, col = %d\n", row, col);
            show_files(names, cnt, row, col);
        }
    }

   // printf("Doing with dir %s\n", dirname);
    return ;
}

void update_color(mode_t mode) {
    bg_c = 0;
    fg_c = 37;
    if (mode & (S_IXUSR | S_IXGRP | S_IXOTH)) fg_c = 32;
    switch (mode & S_IFMT) {
        case S_IFDIR:
            fg_c = 34;
            break;
        case S_IFLNK:
            fg_c = 36;
            break;
    }

}

void mode_to_str(mode_t mode, char *str) {
    if (S_ISREG(mode)) str[0] = '-';
    if (S_ISDIR(mode)) str[0] = 'd';
    if (S_ISCHR(mode)) str[0] = 'c';
    if (S_ISBLK(mode)) str[0] = 'b';
    if (S_ISSOCK(mode)) str[0] = 's';
    if (S_ISLNK(mode)) str[0] = 'l';
    if (S_ISFIFO(mode)) str[0] = 'p';
    
    if (mode & S_IRUSR) str[1] = 'r';
    if (mode & S_IWUSR) str[2] = 'w';
    if (mode & S_IXUSR) str[3] = 'x';

    if (mode & S_IRGRP) str[4] = 'r';
    if (mode & S_IWGRP) str[5] = 'w';
    if (mode & S_IXGRP) str[6] = 'x';

    if (mode & S_IROTH) str[7] = 'r';
    if (mode & S_IWOTH) str[8] = 'w';
    if (mode & S_IXOTH) str[9] = 'x';
    
    //if ((mode & S_IXUSR) && (mode &S_ISUID)) str[3] = 's';

    //处理mode获得背景色和前景色
    update_color(mode);
    return ;
}

char *uid_to_name(uid_t uid) {
    struct passwd *pw_ptr;
    static char tmpstr[10] = {0};
    if ((pw_ptr = getpwuid(uid)) == NULL) {
        sprintf(tmpstr, "%d", uid);
        return tmpstr;
    } else {
        return pw_ptr->pw_name;
    }
}

char *gid_to_name(gid_t gid) {
    struct group *gr_ptr;
    static char tmpstr[10] = {0};
    if ((gr_ptr = getgrgid(gid)) == NULL) {
        sprintf(tmpstr, "%d", gid);
        return tmpstr;
    } else {
        return gr_ptr->gr_name;
    }
}

/*
void print_in_color_l(char *filename, struct stat *info) {
    mode_t mode = info->st_mode;
    if (S_ISDIR(mode)) printf("\033[1;34m%s\033[0m\n", filename);
    else if (S_ISLNK(mode)) printf("\033[0;34m%s\033[0m\n", filename);
    else if (mode & S_IXUSR) printf("\033[1;32m%s\033[0m\n", filename);
    else printf("%s\n", filename);

    return ;
}
*/

//对比我的ls中这个函数，就能看出我设计的有多sb了
void show_info(char *filename, struct stat *info) {
    char modestr[11] = "----------";
    mode_to_str(info->st_mode, modestr);
    printf("%s ", modestr);
    printf("%4ld ", info->st_nlink);
    printf("%-8s ", uid_to_name(info->st_uid));
    printf("%-8s ", gid_to_name(info->st_gid));
    printf("%8ld ", info->st_size);
    printf("%.15s ", 4 + ctime(&info->st_mtime));//4+很重要！为什么要加.15呢？
    printf("\033[%d;%dm%s\033[0m ", bg_c, fg_c, filename);
    //print_in_color_l(filename, info);
    
    if (modestr[0] == 'l') {
        int cnt;
        char buf[NAMEMAX] = {0};//如果不初始化为0，可能会段错误，因为readlink没有'\0'
        if ((cnt = readlink(filename, buf, NAMEMAX)) < 0) {
            //cnt是用来记录字符串长度的，可以去man手册查readlink返回值
            perror("readlink");
        } 
        else {//如果没出错
            printf("-> \033[%d;%dm%s\033[0m", bg_c, fg_c, buf);
        }
    }
    printf("\n");
    return ;
}

void do_stat(char *filename) {
    struct stat st;
    if (lstat(filename, &st) < 0) {
        perror(filename);
    } else {
        show_info(filename, &st);
    }
    return ;
}


void size_window(char filenames[][NAMEMAX], int cnt, int *row, int *col) {
    struct winsize size;
    int len[cnt], max = 0, total = 0;
    memset(len, 0, sizeof(int) * cnt);
    if (isatty(STDOUT_FILENO) == 0) {
        exit(1);
    }
    
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &size) < 0) {
        perror("ioctl");
        exit(1);
    }
    printf("window size row = %d, col = %d\n", size.ws_row, size.ws_col);
    
    for (int i = 0; i < cnt; i++) {
        len[i] = strlen(filenames[i]);
        if (max < len[i]) max = len[i];
        total += len[i] + 1;
    }
    if (max + 1 >= size.ws_col) {
        //如果最长的一个文件名大于整个屏幕宽度
        *row = cnt;
        *col = 1;
        return;
    }
    if (total <= size.ws_col) {
        //如果总长度还是小于屏幕宽度
        *row = 1;
        *col = cnt;
        return ;
    }

    int try_begin = 0;
    for (int i = 0, tmp = 0; i < cnt; i++) {
        tmp += (len[i] + 1);
        if (tmp >= size.ws_col) {
            try_begin = i;
            break;
        } 
    }
    
    for (int i = try_begin; ; i--) {
        int *wide = (int *)malloc(sizeof(int) * i);
        memset(wide, 0, sizeof(int) * i);
        *row = (int)ceil(cnt / i);
        int try_sum = 0;
        for (int x = 0; x < i; x++) {
            for (int y = x * *row; y < (x + 1) * *row && y < cnt; y++) {
                if (wide[x] < len[y]) wide[x] = len[y];
            }
            try_sum += (wide[x] + 1);
        }

        if (try_sum > size.ws_col) continue;
        if (try_sum <= size.ws_col) {
            *col = i;
            break;
        }
    }
}

/*
void print_in_color(char *filename, struct stat *info, int arg) {
    mode_t mode = info->st_mode;
    if (S_ISDIR(mode)) printf("\033[1;34m%-*s\033[0m\n", arg, filename);
    else if (S_ISLNK(mode)) printf("\033[0;34m%-*s\033[0m\n", arg, filename);
    else if (mode & S_IXUSR) printf("\033[1;32m%-*s\033[0m\n", arg, filename);
    else printf("%-*s\n", arg, filename);
}
*/

void show_files(char filename[][NAMEMAX], int cnt, int row, int col) {
    int wide_file[cnt];
    memset(wide_file, 0, sizeof(int) * cnt);
    for (int i = 0; i < col; i++) {
        for (int j = (i * row); j < (i + 1) * row && j < cnt; j++) {
            if (wide_file[i] < strlen(filename[j])) wide_file[i] = strlen(filename[j]);
        }
    }

    for (int i = 0; i < row; i++) {
        for (int j = i; j < (i + row * col) && j < cnt; j = (j + row)) {
            int tmp = j / row;
            int arg = wide_file[tmp] + 1;
            struct stat st;
            if (lstat(filename[j], &st) < 0) {
                perror(filename[j]);
            } else {
                update_color(st.st_mode);
                printf("\033[%d;%dm%-*s\033[0m", bg_c, fg_c, wide_file[tmp] + 1, filename[j]);
            }
        }
        printf("\n");
    }
}
