#include "util.h"

static int THREADPOOL_MAX = 4;

//Return 1 if argument is invalid
int invalidArgument(char * argument)
{
	char * invalidList[] = {"-p","-l","-d","-s","-",".",".."};
	for(int i = 0; i < 7; i++)
	{
		if(strcmp(argument,invalidList[i]) == 0)
		{
			return 1;
			printf("Argument : %s : is invalid\n", argument);
		}

	}
	return 0;
}

void handleArguments(struct settingsdata * settings, int argc, char ** argv)
{
	settings->filepath = NULL;
	settings->listeningport = 8888;
	settings->daemonMode = 0;
	settings->requestHandlingMode = 'P';
	for(int i = 0; i < argc; i++)
	{
		if(strcmp(argv[i],"-p") == 0)
		{
			if(i+1 >= argc)
				printf("%s\n", "No port specified");
			else
			{
				if(!invalidArgument(argv[i+1]))
					settings->listeningport = atoi(argv[i+1]);
			}

		}
		else if(strcmp(argv[i], "-l") == 0)
		{
			if(i+1 >= argc)
				printf("%s\n","No filepath for log specified");
			else
			{
				if(!invalidArgument(argv[i+1]))
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
		else if(strcmp(argv[i], "-d") == 0)
		{
			settings->daemonMode = 1;
		}
		else if(strcmp(argv[i], "-s") == 0)
		{
			if(i+1 >= argc)
			{
				printf("%s\n","No request handling mode set");
			}
			else
			{
				if(!invalidArgument(argv[i+1]))
				{
					if (strcmp(argv[i+1],"fork"))
					{
						settings->requestHandlingMode = 'F';
					}
					if (strcmp(argv[i+1],"thread"))
					{
						settings->requestHandlingMode = 'T';
					}
				}
			}
		}
	}
}

void handleConnection(int client_sock) //TODO add id for threads for basic polling
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
