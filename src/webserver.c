#include <stdio.h>
#include <stdlib.h>

char helptext[] = "-h Print help text\n-p port Listen to port number port\n-d  Run as daemon instead of as a normal program\n-l logfile Log to logfile. If this option is not specified. By default, logging will be output to syslog.\n-s [fork | thread] Select request handling method, thread will be used by default";

void printUsage(char ** argv)
{
	printf("%s%s%s\n\n","Usage : ", argv[0], " flags ");
	printf("%s\n",helptext);
}


int main(int argc, char ** argv)
{
	printUsage(argv);
	return 0;
}
