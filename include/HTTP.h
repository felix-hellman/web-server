#ifndef HTTP_H
#define HTTP_H
#include <stdlib.h>

#define KB(x)   ((size_t) (x) << 10)
#define MB(x)   ((size_t) (x) << 20)
#define MAX_FSIZE MB(2)
#define MAX_HSIZE KB(1)
#define WWW "/var/www"

/*
 *	return codes
 *	0 = Reading done
 *	1 = Reading not done
 */

int HTTP_Request(char *request, char **response, char *buffer, int buffersize, int offset);
int GET(char *request, char *buffer, int buffersize, int offset);
int HEAD(char *request, char *buffer, int buffersize, int offset);
int extractFilename(char *request, char *filepath);
void createHeader(char * header, int length);
//TODO
//openFile() return error if not found or no permission (403&404)
#endif
