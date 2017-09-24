#include "util.h"

static int THREADPOOL_MAX = 16;

//Return 1 if argument is invalid
int invalidArgument(char * argument)
{
	char * invalidList[] = {"-p","-l","-d","-s","-",".",".."};
	for(int i = 0; i < 7; i++)
	{
		if(strcmp(argument,invalidList[i]) == 0)
		{
			return 1;
			printf("Argument : %s is invalid\n", argument);
		}

	}
	return 0;
}

void handleArguments(struct settingsdata * settings,struct configsettings * defaultsettings, int argc, char ** argv)
{
	settings->filepath = NULL;
	settings->daemonMode = 0;
	settings->requestHandlingMode = 'T';
	settings->listeningport = 8080;
	if(defaultsettings->port != 0) //Other default was read from config file
	{
		settings->listeningport = defaultsettings->port;
		settings->requestHandlingMode = defaultsettings->requesthandling[0];
	}

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

void handleConnection(struct thread_data * data)
{
	while(1)
	{
		while(data->clientsocket == 0)
		{
			usleep(1000);
		}
		struct timeval tv;
		tv.tv_sec = 30;  /* 2 Secs Timeout */
		tv.tv_usec = 0;  // Not init'ing this can cause strange errors
		setsockopt(data->clientsocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv,sizeof(struct timeval));
		const int buffersize = 1024;
		char * client_message = calloc(sizeof(char),buffersize);
		char * message = calloc(sizeof(char),buffersize);

		struct HTTP_buffer httpbuff;
		httpbuff.buffer = message;
		httpbuff.buffersize = buffersize;
		httpbuff.response = NULL;
		httpbuff.offset = 0;
		httpbuff.client_message = client_message;
		httpbuff.method = 0;
		httpbuff.raw_path[0] = '\0';
		httpbuff.path[0] = '\0';
		httpbuff.version = -1;
		httpbuff.modified[0] = '\0';
		httpbuff.content_type = 0;

		recv(data->clientsocket , client_message, buffersize, 0);
		
		int offset = 0;
		int returncode = 1;
		do
		{
			memset(message,0,buffersize);
			returncode = HTTP_Request(&httpbuff);
			offset++;
			write(data->clientsocket,message,strlen(message));
		} while(returncode != 0);

		free(message);
		free(client_message);
		shutdown(data->clientsocket,SHUT_RDWR);
		close(data->clientsocket);
		data->clientsocket = 0;
	}
}

void init_thread(pthread_t * thread,struct thread_data * data)
{
	int errorcode = pthread_create(thread,NULL,(void *)handleConnection,(void *) data);
	if(errorcode)
	{
		printf("Error from pthread_create\n");
	}
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
