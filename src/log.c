#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <bsd/string.h>
#include <fcntl.h>
#include "log.h"

void writeToLog(char * filepath, struct HTTP_buffer * HTTP, int socketfd)
{
	const int buffersize = 1024;
	char * buffer = calloc(sizeof(char),buffersize);
	struct log_entry le;
	le.ip[0] = '\0';
	le.date[0] = '\0';
	le.request[0] = '\0';
	le.code[0] = '\0';
	le.bytes[0] = '\0';

	resolveIp(socketfd, &le);	
	logcat(buffer, &le);

	printf("%s\n", buffer);fflush(stdout);
	//writeToFile(filepath,buffer,buffersize);
	free(buffer);
}

void writeToFile(char * filepath, char * buffer, int buffersize)
{
	/*struct flock lock;
	int fd = open(filepath, O_WRONLY | O_APPEND);

	memset(&lock,0,sizeof(lock));
	lock.l_type = F_WRLCK;
	fctnl(fd,F_SETLKW,&lock); //Place lock
	//Write to file
	write(fd,buffer,buffersize);
	//Exit critical section
	lock.l_type = F_UNLCK;
	fcntl(fd,F_SETLKW,&lock); //Unlock
	close(fd);
	*/
}

void resolveIp(int fd, struct log_entry *le)
{
	strlcpy(le->ip, "127.0.0.1 ", 19);
}

void logcat(char *buffer, struct log_entry *le)
{
	strlcat(buffer, le->ip, LOG_ENTRY_SIZE);
	strlcat(buffer, "- - ", LOG_ENTRY_SIZE);
	strlcat(buffer, le->date, LOG_ENTRY_SIZE);
	strlcat(buffer, le->request, LOG_ENTRY_SIZE);
	strlcat(buffer, le->code, LOG_ENTRY_SIZE);
	strlcat(buffer, le->bytes, LOG_ENTRY_SIZE);
}
