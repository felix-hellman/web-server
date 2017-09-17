#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include "structs.h"
#include "HTTP.h"

extern int errno;

int HTTP_Request(struct HTTP_buffer *HTTP)
{
	if (HTTP->response == NULL) {
		HTTP->response = calloc((HEADER_SIZE + FILE_SIZE + 1), sizeof(char));
		return parseRequest(HTTP);
	} else {
		return sendBuffer(HTTP);
	}
}

int parseRequest(struct HTTP_buffer *HTTP)
{
	struct HTTP_request request;
	parseMethod(HTTP->client_message, &request);
	if (request.method == 1 || request.method == 2)
		parsePath(HTTP->client_message, &request);
	if (request.method == 1 || request.method == 2)
		parseVersion(HTTP->client_message, &request);

	switch (request.method) {
	case 1:
		return GET(HTTP, &request);
	case 2:
		return HEAD(HTTP, &request);
	case -1:
		return NOT_IMPL(HTTP);
	case -2:
		return BAD_REQ(HTTP);
	case -3:
		return FORBIDDEN(HTTP);
	case 0:
	default:
		return SERV_ERR(HTTP);
	}
}

void parseMethod(const char *client_message, struct HTTP_request *request)
{
	char tmp[8]; //The longest methods are 7 characthers() eg. CONNECT, OPTIONS
	int i = 0;
	while (client_message[i] != ' ' && i < 7 && i < strlen(client_message)) {
		tmp[i] = client_message[i];
		i++;
	}
	tmp[i] = '\0';
	
	if (strcmp(tmp, "GET") == 0)
		request->method = 1;
	else if (strcmp(tmp, "HEAD") == 0)
		request->method = 1;
	else if (cmpNotImpl(tmp))
		request->method = -1;
	else
		request->method = -2;
}

void parsePath(const char *client_message, struct HTTP_request *request)
{
	//Step past HEAD, GET and spaces to get to the requestedpath
	int stepper;
	if (request->method == 1)
		stepper = 3;
	else 
		stepper = 4;
	while (stepper < strlen(client_message) && client_message[stepper] == ' ')
		stepper++;

	char tmp[PATH_MAX];
	strcpy(tmp, WWW);
	int i = strlen(WWW);
	int j = 0;
	while (client_message[stepper] != ' ' &&
	client_message[stepper] != '\r' &&
	client_message[stepper] != '\n' &&
	client_message[stepper] != '\0' &&
	client_message[stepper] != '\\' &&
	stepper < strlen(client_message) &&
	i < (PATH_MAX - 1 - strlen("index.html") - strlen(WWW))) { //making room for WWW and possible index.html
		tmp[i++] = client_message[stepper];
		request->raw_path[j++] = client_message[stepper];
		stepper++;
	}
	tmp[i] = '\0';

	if (tmp[i-1] == '/')
		strcat(tmp, "index.html");

	char *res = realpath(tmp, request->path);
	if (res == NULL)
		request->method = 0;

	i = 0;
	while (i < strlen(WWW)) {
		if (i >= strlen(tmp) && WWW[i] != tmp[i])
			request->method = -3;
		i++;
	}
}

void parseVersion(const char *client_message, struct HTTP_request *request)
{
	//Step past HEAD, GET, path and spaces to get to the requestedpath
	int stepper;
	if (request->method == 1)
		stepper = 3;
	else 
		stepper = 4;
	while (stepper < strlen(client_message) && client_message[stepper] == ' ')
		stepper++;
	stepper += strlen(request->raw_path);
	while (stepper < strlen(client_message) && client_message[stepper] == ' ')
		stepper++;

	if (client_message[stepper] == '\r' ||
	client_message[stepper] == '\n' ||
	client_message[stepper] == '\0' ||
	client_message[stepper] == '\\') {
		request->version = 9;
	} else {
		if (stepper + 7 >= strlen(client_message) ||
		client_message[stepper] != 'H' || 
		client_message[stepper+1] != 'T' ||
		client_message[stepper+2] != 'T' ||
		client_message[stepper+3] != 'P' ||
		client_message[stepper+4] != '/' ||
		client_message[stepper+6] != '.')
			request->method=-2;
		else if(client_message[stepper+5] != '0' ||
		client_message[stepper+7] != '9')
			request->version=9;
		else if(client_message[stepper+5] != '1' ||
		client_message[stepper+7] != '0')
			request->version=10;
		else if(client_message[stepper+5] != '1' ||
		client_message[stepper+7] != '1')
			request->version=11;
		else
			request->method=0;
	}
}

int GET(struct HTTP_buffer *HTTP, struct HTTP_request *request)
{
	char content[FILE_SIZE] = "";
	readFile(content, request);
	if (request->method == 0)
		return SERV_ERR(HTTP);
	else if (request->method == -4)
		return NOT_FOUND(HTTP);

	int length = strlen(content);
	char header[HEADER_SIZE];
	strcpy(HTTP->response, "");
	if (request->version != 9) {
		createHeader(header, length);
		strcat(HTTP->response, header);
	}
	strcat(HTTP->response, content);

	return sendBuffer(HTTP);
}


int HEAD(struct HTTP_buffer *HTTP, struct HTTP_request *request)
{
	char content[FILE_SIZE] = "";
	readFile(content, request);
	if (request->method == 0)
		return SERV_ERR(HTTP);
	else if (request->method == -4)
		return NOT_FOUND(HTTP);

	int length = strlen(content);
	char header[HEADER_SIZE];
	createHeader(header, length);
	strcpy(HTTP->response, header);

	return sendBuffer(HTTP);
}

int NOT_IMPL(struct HTTP_buffer *HTTP)
{
	strcpy(HTTP->response, "501 Not Implemented\n");
	return sendBuffer(HTTP);
}

int BAD_REQ(struct HTTP_buffer *HTTP)
{
	strcpy(HTTP->response, "400 Bad Request\n");
	return sendBuffer(HTTP);
}

int FORBIDDEN(struct HTTP_buffer *HTTP)
{
	strcpy(HTTP->response, "403 Forbidden\n");
	return sendBuffer(HTTP);
}

int NOT_FOUND(struct HTTP_buffer *HTTP)
{
	strcpy(HTTP->response, "404 Not Found\n");
	return sendBuffer(HTTP);
}

int SERV_ERR(struct HTTP_buffer *HTTP)
{
	strcpy(HTTP->response, "500 Internal Server Error\n");
	return sendBuffer(HTTP);
}

int sendBuffer(struct HTTP_buffer *HTTP)
{
	memcpy(HTTP->buffer, &HTTP->response[HTTP->buffersize * HTTP->offset], HTTP->buffersize);
	if (strlen(HTTP->response) > (HTTP->offset+2)*HTTP->buffersize) {
		return 1;
	} else {
		free(HTTP->response);
		HTTP->response = NULL;
		return 0;
	}
}

void readFile(char *content, struct HTTP_request *request)
{
	FILE *file = fopen(request->path, "r");

	if (file == NULL) {
		int errnum = errno;
		if (strcmp(strerror(errnum), "No such file or directory") == 0) //TODO Forbidden if no permission
			request->method = -4;
		else
			request->method = 0;
	}  else {
		char ch;
		int i = 0;
		while ((ch = fgetc(file)) != EOF && i < (FILE_SIZE-1))
			content[i++] = ch;
		content[i] = '\0';
		fclose(file);
	}
}

//TODO correct date, identify data type
void createHeader(char *header, int length)
{
	strcpy(header, "HTTP/1.0 200 OK\n");
	strcat(header, "Date: Tue, 12 Sep 2017 19:49:32 GMT\nServer: AdamFelix\nContent-Length: ");
	char lengthstr[12];
	sprintf(lengthstr, "%d", length);
	strcat(header, lengthstr);
	strcat(header, "\nContent-Type: text/html\n\n");
}

int cmpNotImpl(char *method)
{
	return (
	strcmp(method, "POST") == 0 ||
	strcmp(method, "PUT") == 0 ||
	strcmp(method, "DELETE") == 0 ||
	strcmp(method, "TRACE") == 0 ||
	strcmp(method, "OPTIONS") == 0 ||
	strcmp(method, "CONNECT") == 0 ||
	strcmp(method, "PATCH") == 0
	);
}
