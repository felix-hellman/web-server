#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


const int THREADPOOL_MAX = 4;

char helptext[] = "-h Print help text\n-p port Listen to port number port\n-d  Run as daemon instead of as a normal program\n-l logfile Log to logfile. If this option is not specified. By default, logging will be output to syslog.\n-s [fork | thread] Select request handling method, thread will be used by default";

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
		something = pthread_create(&threads[i],NULL,printHello, i);
		if(something) //Check for error code
		{
			printf("Error from pthread_create");
			exit(-1);
		}
	}
	threadCleanup(threads);
	printUsage(argv);
	pthread_exit(NULL);


	//Do some socket stuff here lol



	return 0;
}
