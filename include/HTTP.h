#ifndef HTTP_H
#define HTTP_H
#include <stdlib.h>
#include "structs.h"

#define KB(x)   ((size_t) (x) << 10)
#define MB(x)   ((size_t) (x) << 20)
#define FILE_SIZE MB(2)
#define HEADER_SIZE KB(1)
#define WWW "/var/www"

int HTTP_Request(struct HTTP_buffer *HTTP);
int parseRequest(struct HTTP_buffer *HTTP);
void parseMethod(const char *client_message, struct HTTP_request *request);
void parsePath(const char *client_message, struct HTTP_request *request);
void parseVersion(const char *client_message, struct HTTP_request *request);
int GET(struct HTTP_buffer *HTTP, struct HTTP_request *request);
int HEAD(struct HTTP_buffer *HTTP, struct HTTP_request *request);
int NOT_IMPL(struct HTTP_buffer *HTTP);
int BAD_REQ(struct HTTP_buffer *HTTP);
int SERV_ERR(struct HTTP_buffer *HTTP);
int FORBIDDEN(struct HTTP_buffer *HTTP);
int NOT_FOUND(struct HTTP_buffer *HTTP);
int sendBuffer(struct HTTP_buffer *HTTP);
void readFile(char *content, struct HTTP_request *request);
void createHeader(char *header, int length);
void datetime(char *datestring);
int cmpNotImpl(char *method);

#endif
