#ifndef HTTP_H
#define HTTP_H
#include <stdlib.h>

#define MAX_FSIZE 2000000
#define MAX_HSIZE 300
#define WWW "/var/www"

/*
 *	return codes
 *	0 = Reading done
 *	1 = Reading not done
 */

int HTTP_Request(char *client_message, char **response, char *buffer, int buffersize, int offset);
int GET(char * client_message, char * buffer, int buffersize, int offset);
int HEAD(char * client_message, char * buffer, int buffersize, int offset);
char **extractFilepath(char * request);
void createHeader(char * header, int length);
//TODO
//openFile() return error if not found or no permission (403&404)
#endif
