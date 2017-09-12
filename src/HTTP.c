#include <stdlib.h>
#include "HTTP.h"


//Return status code
int GET(char * client_message, char * buffer, int buffersize,int offset)
{
	buffer[0] = 'G';
	buffer[1] = 'E';
	buffer[2] = 'T';

	return 0;
}

int HEAD(char * client_message, char * buffer, int buffersize,int offset)
{
	buffer[0] = 'H';
	buffer[1] = 'E';
	buffer[2] = 'A';
	buffer[3] = 'D';

	return 0;
}
