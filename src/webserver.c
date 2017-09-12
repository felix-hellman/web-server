#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "HTTP.h"
#include "util.h"

int THREADPOOL_MAX = 4;

int main(int argc, char ** argv)
{
	pthread_t threads[THREADPOOL_MAX];
	struct settingsdata settings;
	int socket_desc, client_sock, c;
	struct sockaddr_in server, client;

	handleArguments(&settings,argc,argv);

	socket_desc = socket(AF_INET, SOCK_STREAM,0);
	if(socket_desc == -1)
	{
		printf("Could not create socket");
	}

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(settings.listeningport);

	if(bind(socket_desc,(struct sockaddr *)&server, sizeof(server)) < 0)
	{
		perror("bind failed. Error");
		return 1;
	}
	printf("%s %d\n","Socket created", server.sin_port);

	int threadIndex = 0;
	while(1)
	{
		printf("%s\n","Listening");
		listen(socket_desc, 3);
		c = sizeof(struct sockaddr_in);

		client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
		if(client_sock < 0)
		{
			perror("Accept failed");
			return 1;
		}
		printf("Spawning thread for index %d\n", threadIndex);
		spawn_connection(&threads[threadIndex],client_sock);
		threadIndex = (threadIndex+1)%THREADPOOL_MAX;
	}

	/*Some cleanup stuff*/

	threadCleanup(threads);
	pthread_exit(NULL);
	if(settings.filepath)
		free(settings.filepath);
	return 0;
}
