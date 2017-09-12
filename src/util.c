#include "util.h"

static int THREADPOOL_MAX = 4;

void handleArguments(struct settingsdata * settings, int argc, char ** argv)
{
	settings->filepath = NULL;
	settings->listeningport = 8888;
	for(int i = 0; i < argc; i++)
	{
		if(strcmp(argv[i],"-p") == 0)
		{
			if(i+1 >= argc)
				printf("%s\n", "No port specified");
			else
				settings->listeningport = atoi(argv[i+1]);
		}
		else if(strcmp(argv[i], "-l") == 0)
		{
			if(i+1 >= argc)
				printf("%s\n","No filepath for log specified");
			else
			{
				int size = strlen(argv[i+1]);
				settings->filepath = calloc(sizeof(char),size);
				for(int x = 0; x < size; x++)
				{
					settings->filepath[x] = argv[i+1][x];
				}
				printf("%s\n",settings->filepath);
			}
		}
	}
}

void handleConnection(int client_sock)
{
	const int buffersize = 1024;
	char * client_message = calloc(sizeof(char),buffersize);
	int read_size;
	char * message = calloc(sizeof(char),buffersize);
	while( ( read_size = recv(client_sock , client_message, buffersize, 0)) > 0)
	{
		int offset = 0;
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
	}
	free(message);
}

void spawn_connection(pthread_t * thread, int client_sock)
{
	int errorcode = pthread_create(thread,NULL,(void *)handleConnection,(void *) client_sock);
	if(errorcode)
	{
		printf("Error from pthread_create");
	}
	printf("Done with spawn %d\n", client_sock);
}



void printUsage(char ** argv)
{
	char helptext[] = "-h Print help text\n-p port Listen to port number port\n-d  Run as daemon instead of as a normal program\n-l logfile Log to logfile. If this option is not specified. By default, logging will be output to syslog.\n-s [fork | thread] Select request handling method, thread will be used by default";
	printf("%s%s%s\n\n","Usage : ", argv[0], " flags ");
	printf("%s\n",helptext);
}

void threadCleanup(pthread_t *threads)
{
	for(int i = 0; i < THREADPOOL_MAX; i++)
	{
		pthread_join(threads[i],NULL);
	}
}
