#ifndef HTTP_H
#define HTTP_H
#include <stdlib.h>

/*
 *	return codes
 *	0 = Reading done
 *	1 = Reading not done
 */


int GET(char * buffer, int buffersize);
int HEAD(char * buffer,int buffersize);


#endif
