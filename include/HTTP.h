#ifndef HTTP_H
#define HTTP_H
#include <stdlib.h>
#include "structs.h"

#define KB(x)   ((size_t) (x) << 10)
#define MB(x)   ((size_t) (x) << 20)
#define PATH_SIZE KB(1)
#define FILE_SIZE MB(2)
#define HEADER_SIZE KB(1)
#define WWW "/var/www"

int HTTP_Request(struct HTTP_buffer *HTTP);
int GET(char *request, char *buffer, int buffersize, int offset);
int HEAD(char *request, char *buffer, int buffersize, int offset);
int readFile(char *filepath, char *content);
int extractFilename(char *request, char *filepath);
void createHeader(char *header, int length);

#endif
