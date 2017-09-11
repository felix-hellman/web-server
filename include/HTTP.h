#ifndef HTTP_H
#define HTTP_H
#include <stdlib.h>

/*
 *	return codes
 *	0 = Reading done
 *	1 = Reading not done
 */

int GET(char * client_message, char * buffer, int buffersize, int offset);
int HEAD(char * client_message, char * buffer, int buffersize, int offset);

#endif
