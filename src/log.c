#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <fcntl.h>
#include "log.h"

void writeToLog(char * filepath, HTTP_buffer * HTTP, int socketfd)
{
	const int buffersize = 1024;
	char * buffer = calloc(sizeof(char),buffersize);
	char ip[20];
	resolveIp(socketfd,ip);
	/*TODO
	 * FILL BUFFER HERE
	 */

	writeToFile(filepath,buffer,buffersize);
	free(buffer);
}
void writeToFile(char * filepath, char * buffer, int buffersize)
{
	struct flock lock;
	int fd = open(filepath, O_WRONLY | O_APPEND);

	memset(&lock,0,sizeof(lock));
	lock.l_type = F_WRLCK;
	fctnl(fd,F_SETLKW,&lock); //Place lock
	/*Write to file*/
	write(fd,buffer,buffersize);
	/*Exit critical section*/
	lock.l_type = F_UNLCK;
	fcntl(fd,F_SETLKW,&lock); //Unlock
	close(fd);
}
void resolveIp(int fd, char * buffer)
{
	strlcpy(buffer,"127.0.0.1");
}
