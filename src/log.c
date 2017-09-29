#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#include <bsd/string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/syslog.h>
#include "log.h"

void writeToLog(char * filepath, struct HTTP_buffer * HTTP, char * ipaddress)
{
	const int buffersize = 1024;
	char * buffer = calloc(sizeof(char),buffersize);
	struct log_entry le;
	le.ip = ipaddress;
	le.date[0] = '\0';
	le.request[0] = '\0';
	le.code[0] = '\0';
	le.bytes[0] = '\0';

	resolveRequest(&le, HTTP);
	resolveDate(&le, HTTP);
	resolveCode(&le, HTTP);
	resolveBytes(&le, HTTP);
	logcat(buffer, &le);
	if(filepath)
	{
		writeToFile(filepath,buffer,buffersize);
	}

	else
	{
		openlog ("exampleprog", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);
		syslog(6,buffer,buffersize);
		closelog();
	}

	free(buffer);
}

void writeToFile(char * filepath, char * buffer, int buffersize)
{
	struct flock lock;
	int fd = open(filepath, O_WRONLY | O_APPEND);

	memset(&lock,0,sizeof(lock));
	lock.l_type = F_WRLCK;
	fcntl(fd,F_SETLKW,&lock); //Place lock
	//Write to file
	write(fd,buffer,buffersize);
	//Exit critical section
	lock.l_type = F_UNLCK;
	fcntl(fd,F_SETLKW,&lock); //Unlock
	close(fd);
}

void resolveBytes(struct log_entry *le, struct HTTP_buffer *HTTP)
{
	snprintf(le->bytes, 13, "%zu ", HTTP->content_length);
}

void resolveRequest(struct log_entry *le, struct HTTP_buffer *HTTP)
{
	int i = 0;
	le->request[i] = '"';
	while (i < 898 && HTTP->client_message[i] != '\n' && HTTP->client_message[i] != '\r' &&HTTP->client_message[i] != '\0') {
		le->request[i+1] = HTTP->client_message[i];
		i++;
	}
	le->request[i+1] = '\0';
	strlcat(le->request, "\" ", 900);
}

void resolveDate(struct log_entry *le, struct HTTP_buffer *HTTP)
{
	time_t rawtime;
	time(&rawtime);
	struct tm * timeinfo;
	timeinfo = localtime(&rawtime);
	//timeinfo->tm_hour = timeinfo->tm_hour - timeinfo->tm_gmtoff/60/60; //Timezone correction to GMT
	strftime(le->date, 30, "[%d/%b/%Y:%H:%M:%S %z] ", timeinfo);
}

void resolveCode(struct log_entry *le, struct HTTP_buffer *HTTP)
{
	int code;
	switch(HTTP->method) {
	case 1:
	case 2:
		code = 200;
		break;
	case -1:
		code = 501;
		break;
	case -2:
		code = 400;
		break;
	case -3:
		code = 403;
		break;
	case -4:
		code = 404;
		break;
	case 0:
	default:
		code = 500;
		break;
	}
	snprintf(le->code, 5, "%d ", code);

}

void logcat(char *buffer, struct log_entry *le)
{
	strlcat(buffer, le->ip, LOG_ENTRY_SIZE);
	strlcat(buffer, " - - ", LOG_ENTRY_SIZE);
	strlcat(buffer, le->date, LOG_ENTRY_SIZE);
	strlcat(buffer, le->request, LOG_ENTRY_SIZE);
	strlcat(buffer, le->code, LOG_ENTRY_SIZE);
	strlcat(buffer, le->bytes, LOG_ENTRY_SIZE);
	strlcat(buffer, "\n", LOG_ENTRY_SIZE);
}
