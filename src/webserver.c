#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <bsd/string.h>
#include "HTTP.h"
#include "util.h"
#include "structs.h"
#include "configloader.h"

const int THREADPOOL_MAX = 16;

int * socket_desc_ptr = NULL;
int logpid;

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
	if(logpid == 0)
		exit(0);
	if(res.settings->requestHandlingMode == 't')
	{
		for(int i = 0; i < THREADPOOL_MAX; i++)
			res.t_data[i].working = 0;
		threadCleanup(res.threads,THREADPOOL_MAX);
		//pthread_exit(NULL);
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
	//cleanconfigsettings(&defaultsettings);


	pthread_t threads[THREADPOOL_MAX];
	struct thread_data t_data[THREADPOOL_MAX];

	//fork logging	before jailing
	logpid = fork();
	if (logpid == 0)
	{
		chdir(defaultsettings.rootdirectory);
		mkfifo(pipename, 0666);
		chmod(pipename,S_IWOTH | S_IROTH | S_IRUSR  | S_IWUSR  | S_IRGRP  | S_IWGRP);
		if(real == 0)
		{
			setuid(1000);
			setgid(1000);
		}
		int fd;
		if ((fd = open(pipename, O_CREAT)) < 0)
			perror("Can't create log pipe");
		close(fd);
		if ((fd = open(pipename, O_RDWR)) < 0) {
			perror("Can't read log pipe");
		} else {
			char buf[1024];
			int n;
			openlog ("webserver", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);
			while (1) {
				n = read(fd, buf, 1024);
				if(buf[0] >= '1' && buf[0] <='9')
					syslog(LOG_INFO, buf, n);
				sleep(1);
			}
		}
		exit(0);
	}

	chdir(defaultsettings.rootdirectory);
	chroot(defaultsettings.rootdirectory);

	int socket_desc, client_sock, c;
	struct sockaddr_in server, client;



	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(settings.listeningport);



	if(real != 0)
	{
		printf("This program needs root access\nShutting down\n");
		exit(0);
	}
	if(settings.daemonMode == 1)
	{
		daemonize();
	}
	socket_desc = socket(AF_INET, SOCK_STREAM,0);
	socket_desc_ptr = &socket_desc;
	if(socket_desc == -1)
	{
		printf("Could not create socket\n");
	}
	if(bind(socket_desc,(struct sockaddr *)&server, sizeof(server)) < 0)
	{
		perror("bind failed. Error");
		return 1;
	}
	if(real == 0)
	{
		int f = open(settings.filepath, O_CREAT, S_IWUSR | S_IRGRP | S_IWOTH);
		close(f);
		chown(settings.filepath,1000,1000);
		setuid(1000);
		setgid(1000);
	}

	char * dbg = calloc(sizeof(char),2);
	dbg[0] = '/';
	dbg[1] = '\0';

	if(settings.requestHandlingMode == 't')
	{
		for(int i = 0; i < THREADPOOL_MAX; i++)
		{
			t_data[i].thread_id = i;
			t_data[i].clientsocket = 0;
			t_data[i].working = 1;
			t_data[i].WWW = dbg;
			t_data[i].logpath = settings.filepath;
			t_data[i].address = calloc(sizeof(char),INET6_ADDRSTRLEN);
			init_thread(&threads[i],&t_data[i]);
		}
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
		/*Get the ipv4 address*/
		struct sockaddr_in *s = (struct sockaddr_in *)&client;
		char ipstr[INET6_ADDRSTRLEN];
    inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof ipstr);

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
			strlcpy(t_data[threadIndex].address,ipstr,INET6_ADDRSTRLEN);
			t_data[threadIndex].clientsocket = client_sock; //Assign a socket to the found thread
			client_sock = -1;
		}
		else if(settings.requestHandlingMode == 'f')
		{
			if(fork() != 0)
			{
				handleConnection(client_sock,defaultsettings.rootdirectory,settings.filepath,ipstr);
				exit(0);
			}
		}

	}
	return 0;
}
