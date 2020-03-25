/*************************************************************************
	> File Name: ls.c
	> Author: 少年宇
	> Mail: 
	> Created Time: 2020年03月15日 星期日 20时36分28秒
 ************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <time.h>

int options[2];//用来标记选项

struct file {
    char attr[15];
    int hlinks;
    char username[30];
    char groupname[30];
    int capacity;
    char mtime[20];
    char filename[50];
};

void do_ls(char[], int options[]);
int get_colums(char dirname[], int options[]);
void print_in_columns(char dirname[], int colums, int options[]);
int get_sum_of_entry(char dirname[], int options[]);
void print_list(char dirname[], int options[]);
void get_attr(struct file*, struct dirent*);
void get_hlinks(struct file*, struct dirent*);
void get_username(struct file*, struct dirent*);
void get_groupname(struct file*, struct dirent*);
void get_capacity(struct file*, struct dirent*);
void get_time(struct file*, struct dirent*);
void get_filename(struct file*, struct dirent*);
void sort(struct file*, int);
void get_str(char str[][1000], struct file*, int sum);

#define swap(a, b) {\
    __typeof(a) __temp = (a);\
    (a) = (b); (b) = __temp;\
}

int main(int argc, char *argv[]) {
    char c;
    while ((c = getopt(argc, argv, "al")) != -1) {
        switch (c) {
            case 'a': options[0] = 1;break;
            case 'l': options[1] = 1;break;
            default: return 0;
        }
    }
    if (argc == optind) {
        do_ls(".", options);
    } else {
            for (int i = optind; i < argc; i++) {
            printf("%s:\n", argv[i]);
            do_ls(argv[i], options);
            (i != optind) && printf("\n");
        }
    }
    return 0;
}

void do_ls(char dirname[], int options[]) {
    DIR *dirp;
    struct dirent *direntp;
    
    if ((dirp = opendir(dirname)) == NULL) {
        perror("opendir");
        return;
    }
    int colums = get_colums(dirname, options);
    //printf("colums = %d\n", colums);
    (options[1] == 1) ? print_list(dirname, options) : print_in_columns(dirname, colums, options);
    /*
    while ((direntp = readdir(dirp)) != NULL) {
        if (direntp->d_name[0] == '.') continue;
        printf("%s\n", direntp->d_name);
    }
    */
    closedir(dirp);
    printf("\n");
}

int get_sum_of_entry(char dirname[], int options[]) {
    DIR *dirp = opendir(dirname);
    struct dirent *direntp;
    int sum = 0;
    while ((direntp = readdir(dirp)) != NULL) {
        if ((options[0] == 0) && (direntp->d_name[0] == '.')) continue;
        sum++;
    }
    closedir(dirp);
    return sum;
}

int get_colums(char dirname[], int options[]) {
    DIR *dirp;
    struct dirent *direntp;
    struct winsize size;
    ioctl(STDIN_FILENO, TIOCGWINSZ, &size);
    int max_size = size.ws_col;//terminal目前能放多少列字符
    int colums = 1;//默认是放一列
    int sum_of_entry = get_sum_of_entry(dirname, options);//求总条目数
    //printf("sum_of_entry = %d\n", sum_of_entry);
    for (int i = 2; ; i++) {
        int flag = 0;//来标记是否结束for循环
        int temp_rows = (sum_of_entry % i ? sum_of_entry / i + 1 : sum_of_entry / i);//如果分为i列，需要几行
        //printf("i = %d, temp_rows = %d\n", i, temp_rows);
        
        int max_width = 0;
        int max_of_colum = 0;
        int cnt = 0;
        dirp = opendir(dirname);//打开目录
        while ((direntp = readdir(dirp)) != NULL) {
            //printf("进到循环里了\n");
            if ((options[0] == 0) && (direntp->d_name[0] == '.')) continue;
            if (cnt && (cnt % temp_rows == 0)) {//每遍历完一列
                max_width += (max_of_colum + 2);
                if (max_width == max_size) {
                    flag = 1;//标记一下可以结束，最终列数就是i
                    break;
                } else if (max_width > max_size) {
                    flag = 2;//标记一下可以结束，最终列数是i - 1
                    break;
                }
                max_of_colum = 0;
            }
            int len = strlen(direntp->d_name);
            if (len > max_of_colum) max_of_colum = len;
           cnt++;
        }
        closedir(dirp);//关闭目录
        
        switch (flag) {
            case 0: 
            case 1: { 
                colums = i;
            } break;
            case 2: {
                colums = i - 1;
            } break;
        }
        if (flag) break;//只有flag = 0时才继续增加列
        if (temp_rows == 1) {
            //如果只需要一行，那么就终止，这很重要，否则会死循环！！
            colums = i;
            break;
        }
    }
    return colums;
}

void print_in_columns(char dirname[], int colums, int options[]) {
    DIR *dirp = opendir(dirname);
    struct dirent *direntp;
    int sum_of_entry = get_sum_of_entry(dirname, options);
    //printf("sum_of_entry = %d\n", sum_of_entry);// = 0？？？？
    int rows = (sum_of_entry % colums ? sum_of_entry / colums + 1 : sum_of_entry / colums);
    char table[rows + 5][colums + 5][10000];//放进一个二维数组中，数组每一个元素都是char*
    int i = 0, j = 0;
    int max_of_colum[100] = {0};
    while ((direntp = readdir(dirp)) != NULL) {
        //printf("hello world\n");
        if ((options[0] == 0) && (direntp->d_name[0] == '.')) continue;//隐藏文件暂时不管
        sprintf(table[i][j], "%s", direntp->d_name);
        //printf("%s\n", direntp->d_name);
        //printf("table[%d][%d] = %s\n", i, j, table[i][j]);
        int len = strlen(direntp->d_name);
        if (len > max_of_colum[j]) max_of_colum[j] = len;//得到该列中最长的文件长度
        i++;
        if (i == rows) {
            i = 0;
            j++;
        }
    }
    /*
    for (i = 0; i < rows; i++) {
        for (j = 0; j < colums; j++) {
            printf("%s\n", table[i][j]);
        }
    }
    */
    char ans[rows + 5][10000];//这里还需要修改！！！！！！！！！
    for (i = 0; i < rows; i++) {
        //printf("进到第二个循环\n");
        int k = 0;
        for (j = 0; j < colums; j++) {
            //printf("%s\n", table[i][j]);
            int len = strlen(table[i][j]);
            int offset = (max_of_colum[j] - len);
            k += sprintf(&ans[i][k], "%s", table[i][j]);
            for (int l = 1; l <= offset + 2; l++) {
                k += sprintf(&ans[i][k], " ");
            }
        }
        ans[i][k] = '\0';
    }
    for (i = 0; i < rows; i++) {
        printf("%s\n", ans[i]);
    }
    closedir(dirp);
    return ;
}


void print_list(char dirname[], int options[]) {
    DIR *dirp = opendir(dirname);
    struct dirent *direntp;
    //我的想法是先存起来，拼接成字符串然后输出，后来发现可以不用存！！！！！
    struct file files[1000];
    int i = 0;
    //先获得每个文件的信息
    while ((direntp = readdir(dirp)) != NULL) {
        if ((options[0] == 0) && direntp->d_name[0] == '.') continue;
        get_attr(&(files[i]), direntp);
        get_hlinks(&(files[i]), direntp);
        get_username(&(files[i]), direntp);
        get_groupname(&(files[i]), direntp);
        get_capacity(&(files[i]), direntp);
        get_time(&(files[i]), direntp);
        get_filename(&(files[i]), direntp);
        i++;
    }
    sort(files, i);//排序
    char str[i + 5][1000];
    get_str(str, files, i);//将每个文件结构体转化为字符串
    //输出
    for (int j = 0; j < i; j++) {
        printf("%s\n", str[j]);
    }
    return ;
}

void get_attr(struct file* filep, struct dirent* direntp) {
    struct stat buf;
    stat(direntp->d_name, &buf);
    //判断文件类型
    if (S_ISREG(buf.st_mode)) filep->attr[0] = '-';
    else if (S_ISDIR(buf.st_mode)) filep->attr[0] = 'd';
    else if (S_ISCHR(buf.st_mode)) filep->attr[0] = 'c';
    else if (S_ISBLK(buf.st_mode)) filep->attr[0] = 'b';
    else if (S_ISFIFO(buf.st_mode)) filep->attr[0] = 'p';
    else if (S_ISLNK(buf.st_mode)) filep->attr[0] = 'l';
    else filep->attr[0] = 's';
    //判断user权限
    filep->attr[1] = ((buf.st_mode & S_IRUSR) ? 'r' : '-');
    filep->attr[2] = ((buf.st_mode & S_IWUSR) ? 'w' : '-');
    filep->attr[3] = ((buf.st_mode & S_IXUSR) ? 'x' : '-');
    //判断goup权限
    filep->attr[4] = ((buf.st_mode & S_IRGRP) ? 'r' : '-');
    filep->attr[5] = ((buf.st_mode & S_IWGRP) ? 'w' : '-');
    filep->attr[6] = ((buf.st_mode & S_IXGRP) ? 'x' : '-');
    //判断others权限
    filep->attr[7] = ((buf.st_mode & S_IROTH) ? 'r' : '-');
    filep->attr[8] = ((buf.st_mode & S_IWOTH) ? 'w' : '-');
    filep->attr[9] = ((buf.st_mode & S_IXOTH) ? 'x' : '-');
    printf("%s\n", filep->attr);
    return ;
}

void get_hlinks(struct file *filep, struct dirent *direntp) {
    struct stat buf;
    stat(direntp->d_name, &buf);
    filep->hlinks = (int) buf.st_nlink;
    //printf("hlinks = %d\n", filep->hlinks);
    return ;
}

void get_username(struct file *filep, struct dirent *direntp) {
    struct stat buf;
    stat(direntp->d_name, &buf);
    int uid = buf.st_uid;
    struct passwd *user = getpwuid(uid);
    sprintf(filep->username, "%s", user->pw_name);
    //printf("username = %s\n", filep->username);
    return ;
}

void get_groupname(struct file *filep, struct dirent *direntp) {
    struct stat buf;
    stat(direntp->d_name, &buf);
    int gid = buf.st_gid;
    struct group *grp = getgrgid(gid);
    sprintf(filep->groupname, "%s", grp->gr_name);
    //printf("groupname = %s\n", filep->groupname);
    return ;
}

void get_capacity(struct file *filep, struct dirent *direntp) {
    struct stat buf;
    stat(direntp->d_name, &buf);
    filep->capacity = buf.st_size;
    //printf("capacity = %d\n", filep->capacity);
    return ;
}

void get_time(struct file *filep, struct dirent *direntp) {
    struct stat buf; 
    stat(direntp->d_name, &buf);
    struct tm *t = localtime(&buf.st_mtime);
    sprintf(filep->mtime, "%2d月 %2d %02d:%02d ", t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min);
    return ;
}

void get_filename(struct file *filep, struct dirent *direntp) {
    //struct stat buf;
    //stat(direntp->d_name, &buf);
    int cnt = 0;
    cnt += sprintf(filep->username, "%s", direntp->d_name);
    filep->filename[cnt] = ((filep->attr[0] == 'd') ? '/' : ' ');
    return ;
}

void sort(struct file *filep, int n) {
    //insert_sort
    for (int i = 1; i < n; i++) {
        for (int j = i; j > 0 && (strcmp(filep[j].filename, filep[j - 1].filename) < 0); j--) {
            swap(filep[j], filep[j - 1]);
        }
    }
    return ;
}

void get_str(char str[][1000], struct file *filep, int n) {
    for (int i = 0; i < n; i++) {
        int cnt = 0;
        cnt += sprintf(str[i], "%s", filep[i].attr);
        //hlinks
        str[i][11] = (filep[i].hlinks >= 100 ? filep[i].hlinks / 100 + '0' : '0');
        str[i][12] = (filep[i].hlinks >= 10 ? filep[i].hlinks % 100 / 10 + '0' : '0');
        str[i][13] = (filep[i].hlinks >= 0 ? filep[i].hlinks % 10 + '0' : '0');
        //username
        str[i][cnt] = ' ';
        cnt = 15;
        cnt += sprintf(&str[i][cnt], "%s", filep[i].username);
        //groupname
        str[i][cnt] = ' ';
        cnt++;
        cnt += sprintf(&str[i][cnt], "%s", filep[i].groupname);
        //capacity
        for (int j = 1; j <= 5; j++) {
            str[i][cnt] = ' ';
            cnt++;      
        }
        str[i][cnt++] = (filep[i].capacity >= 10000 ? filep[i].capacity / 10000 + '0' : '0');
        str[i][cnt++] = (filep[i].capacity >= 1000 ? filep[i].capacity % 10000 / 1000 + '0' : '0');
        str[i][cnt++] = (filep[i].capacity >= 100 ? filep[i].capacity % 1000 / 100 + '0' : '0');
        str[i][cnt++] = (filep[i].capacity >= 10 ? filep[i].capacity % 100 / 10 + '0' : '0');
        str[i][cnt++] = (filep[i].capacity >= 0 ? filep[i].capacity % 10 + '0' : '0');
        //time
        str[i][cnt] = ' ';
        cnt++;
        cnt += sprintf(&str[i][cnt], "%s", filep[i].mtime);
        //filename
        str[i][cnt] = ' ';
        cnt++;
        cnt += sprintf(&str[i][cnt], "%s", filep[i].filename);
        str[i][cnt] = '\n';
        str[i][cnt] = '\0';
    }
    return ;
}

