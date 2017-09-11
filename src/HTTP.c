#include <stdlib.h>
#include "../include/HTTP.h"


//Return status code
int GET(char * buffer, int buffersize)
{
	buffer[0] = 'G';
	buffer[1] = 'E';
	buffer[2] = 'T';

	return 1;
}

int HEAD(char * buffer, int buffersize)
{
	buffer[0] = 'H';
	buffer[1] = 'E';
	buffer[2] = 'A';
	buffer[3] = 'D';
	
	return 1;
}
