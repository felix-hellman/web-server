#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/wait.h>
#include "HTTP.h"
#include "util.h"
#include "structs.h"
#include "configloader.h"

const int THREADPOOL_MAX = 16;

int * socket_desc_ptr = NULL;

struct resources
{
	int listeningsocket;
	struct settingsdata * settings;
	pthread_t * threads;
	struct thread_data * t_data;
};

struct resources res;

/**
 * @brief Handles sig_int and makes sure the server handle remaining requests
 */
void handle_signal(int signal)
{
	printf("Signal : %d\n",signal);

	/*Some cleanup stuff*/
	if(res.settings->requestHandlingMode == 't')
	{
		for(int i = 0; i < THREADPOOL_MAX; i++)
			res.t_data[i].working = 0;
		threadCleanup(res.threads,THREADPOOL_MAX);
		pthread_exit(NULL);
	}
	else if (res.settings->requestHandlingMode == 'f')
	{
		wait(NULL);
	}
	if(res.settings->filepath)
		free(res.settings->filepath);
	shutdown(res.listeningsocket,SHUT_WR);
	exit(0);
	//close(*socket_desc_ptr);
}
/**
 * @brief Listens to listening port and dispatches work when a connection is established
 */
int main(int argc, char ** argv)
{

	int real = getuid();

	struct configsettings defaultsettings;
	loadconfiguration(&defaultsettings);
	struct settingsdata settings;
   	struct sigaction sa;
	sa.sa_handler = &handle_signal;
	sigfillset(&sa.sa_mask);

	if(sigaction(SIGINT, &sa, NULL) == -1)
	{
		perror("Can't handle sigint");
	}

	handleArguments(&settings,&defaultsettings,argc,argv);
	cleanconfigsettings(&defaultsettings);
	if(settings.daemonMode == 1)
	{
		int pid = fork();
		close(STDOUT_FILENO);
		close(STDERR_FILENO);
		if(pid != 0)
			exit(0);
	}

	pthread_t threads[THREADPOOL_MAX];
	struct thread_data t_data[THREADPOOL_MAX];

	if(settings.requestHandlingMode == 't')
	{
		for(int i = 0; i < THREADPOOL_MAX; i++)
		{
			t_data[i].working = 0;
			t_data[i].thread_id = i;
			t_data[i].clientsocket = 0;
			t_data[i].working = 1;
			init_thread(&threads[i],&t_data[i]);
		}
	}

	chdir(defaultsettings.rootdirectory);
	chroot(defaultsettings.rootdirectory);

	int socket_desc, client_sock, c;
	struct sockaddr_in server, client;

	socket_desc = socket(AF_INET, SOCK_STREAM,0);
	socket_desc_ptr = &socket_desc;
	if(socket_desc == -1)
	{
		printf("Could not create socket\n");
	}

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(settings.listeningport);

	if(bind(socket_desc,(struct sockaddr *)&server, sizeof(server)) < 0)
	{
		perror("bind failed. Error");
		return 1;
	}

	if(!real)
	{
		setuid(1000);
		setgid(1000);
	}
	else
	{
		printf("This program needs root access\nShutting down\n");
		exit(0);
	}


	int threadIndex = 0;
	res.settings = &settings;
	res.listeningsocket = socket_desc;
	res.threads = threads;
	res.t_data = t_data;
	while(1)
	{
		listen(socket_desc, 50);
		c = sizeof(struct sockaddr_in);
		client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c); //Recieve connection
		if(client_sock < 0)
		{
			perror("Accept failed");
			return 1;
		}
		if(settings.requestHandlingMode == 't')
		{
			while(t_data[threadIndex].clientsocket != 0) //Find a free thread
			{
				threadIndex = (threadIndex+1)%THREADPOOL_MAX;
			}
			t_data[threadIndex].clientsocket = client_sock; //Assign a socket to the found thread
			client_sock = -1;
		}
		else if(settings.requestHandlingMode == 'f')
		{
			int forkid = fork();
			if(forkid != 0)
			{
				handleConnection(client_sock);
				exit(0);
			}
		}

	}
	return 0;
}
