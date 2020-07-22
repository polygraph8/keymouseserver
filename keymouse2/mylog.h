#pragma once
#include <time.h>
#include <sys/timeb.h>
#include <stdio.h>
#include <atlstr.h>

FILE* logfd = NULL;
char fn1[10] = "1.log";
char fn2[10] = "2.log";
char fn[10] = "1.log";
int filesizelimit = 10000000;


void mylog(char *buf, char* ip) {
	printf(buf);
	errno_t err;
	err = fopen_s(&logfd, fn, "a+");

	struct tm local;
	time_t t;
	t = time(NULL);
	err = localtime_s(&local, &t);
	fprintf_s(logfd, "%d-%d-%d %d:%d:%d :%s:  %s\n",
		local.tm_year + 1900, local.tm_mon + 1, local.tm_mday, local.tm_hour, local.tm_min, local.tm_sec, ip, buf);
	int size = ftell(logfd);
	if (size > filesizelimit) {
		if (strcmp(fn, fn1) == 0)
			strcpy_s(fn, fn2);
		else
			strcpy_s(fn, fn1);
		fclose(logfd);
		err = fopen_s(&logfd, fn, "w+");
		fputs("change file ...\n", logfd);
	}
	fflush(logfd);
	fclose(logfd);
}

