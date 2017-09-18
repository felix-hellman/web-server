#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#include <errno.h>
#include <sys/stat.h>
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
		return NOT_IMPL(HTTP, &request);
	case -2:
		return BAD_REQ(HTTP, &request);
	case -3:
		return FORBIDDEN(HTTP, &request);
	case -4:
		return NOT_FOUND(HTTP, &request);
	case 0:
	default:
		return SERV_ERR(HTTP, &request);
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
		request->method = 2;
	else if (cmpNotImpl(tmp))
		request->method = -1;
	else
		request->method = -2;
}

void parsePath(const char *client_message, struct HTTP_request *request)
{
	//Step past HEAD, GET and spaces to get to the requested path
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
	if (client_message[stepper] != '/')
		tmp[i++] = '/';
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

	//Resolve relative paths
	char *res = realpath(tmp, request->path);
	if (res == NULL) {
		int errnum = errno;
		if (strcmp(strerror(errnum), "No such file or directory") == 0)
			request->method = -4;
		else
			request->method = 0;
	}

	//Make sure the resolved path is still in WWW folder
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
			request->method = -2;
		else if(client_message[stepper+5] == '0' &&
		client_message[stepper+7] == '9')
			request->version = 9;
		else if(client_message[stepper+5] == '1' &&
		client_message[stepper+7] == '0')
			request->version = 10;
		else if(client_message[stepper+5] == '1' &&
		client_message[stepper+7] == '1')
			request->version = 11;
		else
			request->method = 0;
	}
}

int GET(struct HTTP_buffer *HTTP, struct HTTP_request *request)
{
	char content[FILE_SIZE] = "";
	readFile(content, request);
	if (request->method == 0)
		return SERV_ERR(HTTP, request);
	else if (request->method == -3)
		return FORBIDDEN(HTTP, request);

	int length = strlen(content);
	char header[HEADER_SIZE];
	strcpy(HTTP->response, "");
	if (request->version != 9) {
		createHeader(header, length, request);
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
		return SERV_ERR(HTTP, request);
	else if (request->method == -3)
		return FORBIDDEN(HTTP, request);

	int length = strlen(content);
	char header[HEADER_SIZE];
	createHeader(header, length, request);
	strcpy(HTTP->response, header);

	return sendBuffer(HTTP);
}

int NOT_IMPL(struct HTTP_buffer *HTTP, struct HTTP_request *request)
{
	char content[FILE_SIZE] = "501 Not Implemented\n";
	int length = strlen(content);
	char header[HEADER_SIZE];
	strcpy(HTTP->response, "");
	if (request->version != 9) {
		createHeader(header, length, request);
		strcat(HTTP->response, header);
	}
	strcat(HTTP->response, content);
	return sendBuffer(HTTP);
}

int BAD_REQ(struct HTTP_buffer *HTTP, struct HTTP_request *request)
{
	char content[FILE_SIZE] = "400 Bad Request\n";
	int length = strlen(content);
	char header[HEADER_SIZE];
	strcpy(HTTP->response, "");
	if (request->version != 9) {
		createHeader(header, length, request);
		strcat(HTTP->response, header);
	}
	strcat(HTTP->response, content);
	return sendBuffer(HTTP);
	return sendBuffer(HTTP);
}

int FORBIDDEN(struct HTTP_buffer *HTTP, struct HTTP_request *request)
{
	char content[FILE_SIZE] = "403 Forbidden\n";
	int length = strlen(content);
	char header[HEADER_SIZE];
	strcpy(HTTP->response, "");
	if (request->version != 9) {
		createHeader(header, length, request);
		strcat(HTTP->response, header);
	}
	strcat(HTTP->response, content);
	return sendBuffer(HTTP);
	return sendBuffer(HTTP);
}

int NOT_FOUND(struct HTTP_buffer *HTTP, struct HTTP_request *request)
{
	char content[FILE_SIZE] = "404 Not Found\n";
	int length = strlen(content);
	char header[HEADER_SIZE];
	strcpy(HTTP->response, "");
	if (request->version != 9) {
		createHeader(header, length, request);
		strcat(HTTP->response, header);
	}
	strcat(HTTP->response, content);
	return sendBuffer(HTTP);
	return sendBuffer(HTTP);
}

int SERV_ERR(struct HTTP_buffer *HTTP, struct HTTP_request *request)
{
	char content[FILE_SIZE] = "500 Internal Server Error\n";
	int length = strlen(content);
	char header[HEADER_SIZE];
	strcpy(HTTP->response, "");
	if (request->version != 9) {
		createHeader(header, length, request);
		strcat(HTTP->response, header);
	}
	strcat(HTTP->response, content);
	return sendBuffer(HTTP);
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
		if (strcmp(strerror(errnum), "Permission denied") == 0)
			request->method = -3;
		else
			request->method = 0;
	}  else {
		char ch;
		int i = 0;
		while ((ch = fgetc(file)) != EOF && i < (FILE_SIZE-1))
			content[i++] = ch;
		content[i] = '\0';

		char datestring[37] = "";
		struct stat mod;
		if (!stat(request->path, &mod)) {
			datetime(datestring, &mod.st_ctime);
		}
		strcpy(request->modified, datestring);
		fclose(file);
	}
}

void createHeader(char *header, int length, struct HTTP_request *request)
{
	strcpy(header, "HTTP/1.0 ");
	switch (request->method) {
	case 1:
	case 2:
		strcat(header, "200 OK\r\nLast-Modified: ");
		strcat(header, request->modified);
		strcat(header, "\r\nContent-Length: ");
		char lengthstr[12];
		sprintf(lengthstr, "%d\r\nDate: ", length);
		strcat(header, lengthstr);
		break;
	case -1:
		strcat(header, "501 Not Implemented\r\nDate: ");
		break;
	case -2:
		strcat(header, "400 Bad Request\r\nDate: ");
		break;
	case -3:
		strcat(header, "403 Forbidden\r\nDate: ");
		break;
	case -4:
		strcat(header, "404 Not Found\r\nDate: ");
		break;
	case 0:
	default:
		strcat(header, "500 Internal Server Error\r\nDate: ");
		break;
	}

	char datestring[37]; //Needs to be 36 + 1 ( LEN + 1 for \0 )
	time_t rawtime;
	time(&rawtime);
	datetime(datestring, &rawtime);
	strcat(header, datestring);
	strcat(header, "\r\nServer: AdamFelix\r\nContent-Type: text/html\r\n\r\n");
}

void datetime(char *datestring, const time_t *timestamp)
{
	struct tm * timeinfo;
	timeinfo = localtime(timestamp);
	timeinfo->tm_hour = timeinfo->tm_hour - timeinfo->tm_gmtoff/60/60; //Timezone correction to GMT
	strftime(datestring, 37, "%a, %d %b %Y %H:%M:%S GMT", timeinfo);
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
