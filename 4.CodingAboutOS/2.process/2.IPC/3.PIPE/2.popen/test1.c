/*************************************************************************
	> File Name: test.c
	> Author: 少年宇
	> Mail: 
	> Created Time: 2020年04月09日 星期四 14时40分54秒
 ************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_SIZE 1024
#define SHELL "/bin/bash"

FILE *my_popen(const char *command, const char *type);

int main(){
    FILE *fp1 = NULL;
    FILE *fp2 = NULL;
    char buff[1024] = {0};
    fp1 = my_popen("cat","w");
    fp2 = fopen("c.log","r");
    fread(buff,1,1024,fp2);
    fwrite(buff,1,1024,fp1);
    pclose(fp1);
    fclose(fp2);
    return 0;
}

FILE *
my_popen(const char *cmdstring, const char *type)
{
	int		i, pfd[2];
	pid_t	pid;
	FILE	*fp;

			/* only allow "r" or "w" */
	if ((type[0] != 'r' && type[0] != 'w') || type[1] != 0) {
		//errno = EINVAL;		/* required by POSIX.2 */
		return(NULL);
	}

	//if (childpid == NULL) {		/* first time through */
				/* allocate zeroed out array for child pids */
		//maxfd = open_max();
		//if ( (childpid = calloc(maxfd, sizeof(pid_t))) == NULL)
			//return(NULL);
	//}

	if (pipe(pfd) < 0)
		return(NULL);	/* errno set by pipe() */

	if ( (pid = fork()) < 0)
		return(NULL);	/* errno set by fork() */
	else if (pid == 0) {							/* child */
		if (*type == 'r') {
			close(pfd[0]);
			if (pfd[1] != STDOUT_FILENO) {
				dup2(pfd[1], STDOUT_FILENO);
				close(pfd[1]);
			}
		} else {
			close(pfd[1]);
			if (pfd[0] != STDIN_FILENO) {
				dup2(pfd[0], STDIN_FILENO);
				close(pfd[0]);
			}
		}
			/* close all descriptors in childpid[] */
		for (i = 0; i < maxfd; i++)
			if (childpid[ i ] > 0)
				close(i);

		execl(SHELL, "sh", "-c", cmdstring, (char *) 0);
		_exit(127);
	}
								/* parent */
	if (*type == 'r') {
		close(pfd[1]);
		if ( (fp = fdopen(pfd[0], type)) == NULL)
			return(NULL);
	} else {
		close(pfd[0]);
		if ( (fp = fdopen(pfd[1], type)) == NULL)
			return(NULL);
	}
	childpid[fileno(fp)] = pid;	/* remember child pid for this fd */
	return(fp);
}

