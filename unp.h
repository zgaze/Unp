/* ***********************************************************************

  > File Name: unp.h
  > Author: zzy
  > Mail: 942744575@qq.com 
  > Created Time: Tue 28 Nov 2017 10:23:33 AM CST

 ********************************************************************** */

#ifndef __UNP_H__
#define __UNP_H__

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/stat.h>

#define MAX_LOG_BUF 1024

void err_exit(const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	char buf[MAX_LOG_BUF];
	vsnprintf(buf, MAX_LOG_BUF, fmt, ap) ;
	strcat(buf, "\n");

	//printf("%s",buf); //和下面三行代码等效
	fflush(stdout);
	fputs(buf, stderr);
	fflush(NULL);
	va_end(ap);
	exit(1);
}

void err_ret(const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	char buf[MAX_LOG_BUF];
	vsnprintf(buf, MAX_LOG_BUF, fmt, ap) ;
	if(errno){
		snprintf(buf + strlen(buf), MAX_LOG_BUF - strlen(buf)-1 ," : %s", strerror(errno));
	}
	strcat(buf, "\n");

	//printf("%s",buf); //和下面三行代码等效
	fflush(stdout);
	fputs(buf, stderr);
	fflush(NULL);
	va_end(ap);
}

void pr_exit(int status)
{
	if(WIFEXITED(status)){
		printf("normal termination, exit status = %d\n" , WEXITSTATUS(status));
	} else if (WIFSIGNALED(status)){
		printf("abnormal termination,singal number = %d%s\n",WTERMSIG(status),
#ifdef WCOREDUMP
			WCOREDUMP(status) ? "(core file generated)" :"");
#else
			"");
#endif
	} else if (WIFSTOPPED(status)){
		printf("child stopped, singal number is %d\n" , WSTOPSIG(status));
	}
}

#endif  //__UNP_H__

