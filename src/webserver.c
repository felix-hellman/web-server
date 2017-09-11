#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "../include/HTTP.h"

const int THREADPOOL_MAX = 4;

char helptext[] = "-h Print help text\n-p port Listen to port number port\n-d  Run as daemon instead of as a normal program\n-l logfile Log to logfile. If this option is not specified. By default, logging will be output to syslog.\n-s [fork | thread] Select request handling method, thread will be used by default";

void handleConnection(int client_sock)
{
	const int buffersize = 1024;
	char * client_message = calloc(sizeof(char),buffersize);
	int read_size;
	while( ( read_size = recv(client_sock , client_message, buffersize, 0)) > 0)
	{
		int offset = 0;
		char * message = calloc(sizeof(char),buffersize);
		int returncode = 1;
		if(client_message[0] == 'H')
		{
			printf("%s\n","HEAD REQUEST");
			while(returncode)
			{
				returncode = HEAD(client_message,message,buffersize,offset++);
				write(client_sock,message,strlen(message));
			}
		}
		else if(client_message[0] == 'G')
		{
			printf("%s\n","GET REQUEST");
			while(returncode)
			{
				returncode = GET(client_message,message,buffersize,offset++);
				write(client_sock,message,strlen(message));
			}
		}

		free(message);
	}
}


void printUsage(char ** argv)
{
	printf("%s%s%s\n\n","Usage : ", argv[0], " flags ");
	printf("%s\n",helptext);
}

void *printHello(int number)
{
	printf("Hello from thread %d\n",number);
}

void threadCleanup(pthread_t *threads)
{
	for(int i = 0; i < THREADPOOL_MAX; i++)
	{
		pthread_join(threads[i],NULL);
	}
}

int main(int argc, char ** argv)
{
	pthread_t threads[THREADPOOL_MAX];
	int threadID, something;
	for(int i = 0; i < THREADPOOL_MAX; i++)
	{
		something = pthread_create(&threads[i],NULL,(void *)printHello,(void *)i);
		if(something) //Check for error code
		{
			printf("Error from pthread_create");
			exit(-1);
		}
	}
	

	//Do some socket stuff here lol
	int socket_desc, client_sock, c, read_size;
	struct sockaddr_in server, client;
	char client_message[2000];

	socket_desc = socket(AF_INET, SOCK_STREAM,0);
	if(socket_desc == -1)
	{
		printf("Could not create socket");
	}

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(8888);

	if(bind(socket_desc,(struct sockaddr *)&server, sizeof(server)) < 0)
	{
		perror("bind failed. Error");
		return 1;
	}
	printf("%s\n","Socket created");

	
	threadCleanup(threads);

	listen(socket_desc, 3);
	c = sizeof(struct sockaddr_in);

	//Creation ends here
	//Output should be client_sock and client_message


	client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
	if(client_sock < 0)
	{
		perror("Accept failed");
		return 1;
	}
	handleConnection(client_sock);
	/*while( ( read_size = recv(client_sock , client_message, 2000, 0)) > 0)
	{
		const int buffersize = 1024;
		int offset = 0;
		char * message = calloc(sizeof(char),buffersize);
		if(client_message[0] == 'H')
		{
			printf("%s\n","HEAD REQUEST");
			
			while(HEAD(client_message,message,buffersize,offset++))
				write(client_sock,message,strlen(message));
		}

		if(client_message[0] == 'G')
		{
			printf("%s\n","GET REQUEST");
			while(GET(client_message,message,buffersize,offset++))
				write(client_sock,message,strlen(message));
		}

		free(message);
	}*/


	printUsage(argv);
	pthread_exit(NULL);
	return 0;
}
