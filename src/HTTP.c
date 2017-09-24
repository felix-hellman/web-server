#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#include <errno.h>
#include <bsd/string.h>
#include <sys/stat.h>
#include "structs.h"
#include "HTTP.h"
extern int errno;

int HTTP_Request(struct HTTP_buffer *HTTP)
{
	if (HTTP->response == NULL) {
		HTTP->response = calloc((RESPONSE_SIZE), sizeof(char));
		HTTP->response[0] = '\0';
		return parseRequest(HTTP);
	} else {
		return sendBuffer(HTTP);
	}
}

int parseRequest(struct HTTP_buffer *HTTP)
{
	parseMethod(HTTP);
	if (HTTP->method == 1 || HTTP->method == 2)
		parsePath(HTTP);
	if (HTTP->method == 1 || HTTP->method == 2)
		parseVersion(HTTP);
	switch (HTTP->method) {
	case 1:
		return GET(HTTP);
	case 2:
		return HEAD(HTTP);
	default:
		return ERROR(HTTP);
	}
}

void parseMethod(struct HTTP_buffer *HTTP)
{
	char tmp[8]; //The longest methods are 7 characthers() eg. CONNECT, OPTIONS
	int i = 0;
	while (HTTP->client_message[i] != ' ' && i < 7 && i < strlen(HTTP->client_message)) {
		tmp[i] = HTTP->client_message[i];
		i++;
	}
	tmp[i] = '\0';

	if (strcmp(tmp, "GET") == 0)
		HTTP->method = 1;
	else if (strcmp(tmp, "HEAD") == 0)
		HTTP->method = 2;
	else if (cmpNotImpl(tmp))
		HTTP->method = -1;
	else
		HTTP->method = -2;
}

void parsePath(struct HTTP_buffer *HTTP)
{
	//Step past HEAD, GET and spaces to get to the HTTPed path
	int stepper;
	if (HTTP->method == 1)
		stepper = 3;
	else
		stepper = 4;
	while (stepper < strlen(HTTP->client_message) && HTTP->client_message[stepper] == ' ')
		stepper++;

	char tmp[PATH_MAX];
	strlcpy(tmp, WWW, sizeof(tmp));
	int i = strlen(WWW);
	if (HTTP->client_message[stepper] != '/')
		tmp[i++] = '/';
	int j = 0;
	while (HTTP->client_message[stepper] != ' ' &&
	HTTP->client_message[stepper] != '\r' &&
	HTTP->client_message[stepper] != '\n' &&
	HTTP->client_message[stepper] != '\0' &&
	HTTP->client_message[stepper] != '\\' &&
	stepper < strlen(HTTP->client_message) &&
	i < (PATH_MAX - 1 - strlen("index.html") - strlen(WWW))) { //making room for WWW and possible index.html
		tmp[i++] = HTTP->client_message[stepper];
		HTTP->raw_path[j++] = HTTP->client_message[stepper];
		stepper++;
	}
	tmp[i] = '\0';

	if (tmp[i-1] == '/')
		strlcat(tmp, "index.html", sizeof(tmp));

	//Resolve relative paths
	char *res = realpath(tmp, HTTP->path);
	if (res == NULL) {
		int errnum = errno;
		if (strcmp(strerror(errnum), "No such file or directory") == 0)
			HTTP->method = -4;
		else
			HTTP->method = 0;
	}

	//Make sure the resolved path is still in WWW folder
	i = 0;
	while (i < strlen(WWW)) {
		if (i >= strlen(tmp) && WWW[i] != tmp[i])
			HTTP->method = -3;
		i++;
	}
}

void parseVersion(struct HTTP_buffer *HTTP)
{
	//Step past HEAD, GET, path and spaces to get to the HTTPedpath
	int stepper;
	if (HTTP->method == 1)
		stepper = 3;
	else
		stepper = 4;
	while (stepper < strlen(HTTP->client_message) && HTTP->client_message[stepper] == ' ')
		stepper++;
	stepper += strlen(HTTP->raw_path);
	while (stepper < strlen(HTTP->client_message) && HTTP->client_message[stepper] == ' ')
		stepper++;

	if (HTTP->client_message[stepper] == '\r' ||
	HTTP->client_message[stepper] == '\n' ||
	HTTP->client_message[stepper] == '\0' ||
	HTTP->client_message[stepper] == '\\') {
		HTTP->version = 9;
	} else {
		if (stepper + 7 >= strlen(HTTP->client_message) ||
		HTTP->client_message[stepper] != 'H' ||
		HTTP->client_message[stepper+1] != 'T' ||
		HTTP->client_message[stepper+2] != 'T' ||
		HTTP->client_message[stepper+3] != 'P' ||
		HTTP->client_message[stepper+4] != '/' ||
		HTTP->client_message[stepper+6] != '.')
			HTTP->method = -2;
		else if(HTTP->client_message[stepper+5] == '0' &&
		HTTP->client_message[stepper+7] == '9')
			HTTP->version = 9;
		else if(HTTP->client_message[stepper+5] == '1' &&
		HTTP->client_message[stepper+7] == '0')
			HTTP->version = 10;
		else if(HTTP->client_message[stepper+5] == '1' &&
		HTTP->client_message[stepper+7] == '1')
			HTTP->version = 11;
		else
			HTTP->method = 0;
	}
}

int GET(struct HTTP_buffer *HTTP)
{
	char content[FILE_SIZE] = "";
	readFile(content, HTTP);
	if (HTTP->method != 1)
		return ERROR(HTTP);

	int length = strlen(content);
	char header[HEADER_SIZE];
	if (HTTP->version != 9) {
		createHeader(header, length, HTTP);
		strlcat(HTTP->response, header, RESPONSE_SIZE);
	}
	strlcat(HTTP->response, content, RESPONSE_SIZE);

	return sendBuffer(HTTP);
}


int HEAD(struct HTTP_buffer *HTTP)
{
	char content[FILE_SIZE] = "";
	readFile(content, HTTP);
	if (HTTP->method != 2)
		return ERROR(HTTP);

	int length = strlen(content);
	char header[HEADER_SIZE];
	createHeader(header, length, HTTP);
	strlcpy(HTTP->response, header, RESPONSE_SIZE);

	return sendBuffer(HTTP);
}

int ERROR(struct HTTP_buffer *HTTP)
{
	char content[FILE_SIZE];
	switch(HTTP->method) {
	case -1:
		strlcpy(content, "501 Not Implemented\n", sizeof(content));
		break;
	case -2:
		strlcpy(content, "400 Bad Request\n", sizeof(content));
		break;
	case -3:
		strlcpy(content, "403 Forbidden\n", sizeof(content));
		break;
	case -4:
		strlcpy(content, "404 Not Found\n", sizeof(content));
		break;
	case 0:
	default:
		strlcpy(content, "500 Internal Server Error\n", sizeof(content));
		break;
	}
	int length = strlen(content);
	char header[HEADER_SIZE];
	if (HTTP->version != 9) {
		createHeader(header, length, HTTP);
		strlcat(HTTP->response, header, FILE_SIZE);
	}
	strlcat(HTTP->response, content, FILE_SIZE);
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

void readFile(char *content, struct HTTP_buffer *HTTP)
{
	FILE *file = fopen(HTTP->path, "r");

	if (file == NULL) {
		int errnum = errno;
		if (strcmp(strerror(errnum), "Permission denied") == 0)
			HTTP->method = -3;
		else
			HTTP->method = 0;
	}  else {
		char ch;
		int i = 0;
		while ((ch = fgetc(file)) != EOF && i < (FILE_SIZE-1))
			content[i++] = ch;
		content[i] = '\0';

		char datestring[DATE_SIZE] = "";
		struct stat mod;
		if (!stat(HTTP->path, &mod)) {
			datetime(datestring, &mod.st_ctime);
		}
		strlcpy(HTTP->modified, datestring, DATE_SIZE);
		fclose(file);
	}
}

void createHeader(char *header, int length, struct HTTP_buffer *HTTP)
{
	strlcpy(header, "HTTP/1.0 ", HEADER_SIZE);
	switch (HTTP->method) {
	case 1:
	case 2:
		strlcat(header, "200 OK\r\nLast-Modified: ", HEADER_SIZE);
		strlcat(header, HTTP->modified, HEADER_SIZE);
		strlcat(header, "\r\nContent-Length: ", HEADER_SIZE);
		char lengthstr[12];
		snprintf(lengthstr, 13, "%d\r\nDate: ", length);
		strlcat(header, lengthstr, HEADER_SIZE);
		break;
	case -1:
		strlcat(header, "501 Not Implemented\r\nDate: ", HEADER_SIZE);
		break;
	case -2:
		strlcat(header, "400 Bad Request\r\nDate: ", HEADER_SIZE);
		break;
	case -3:
		strlcat(header, "403 Forbidden\r\nDate: ", HEADER_SIZE);
		break;
	case -4:
		strlcat(header, "404 Not Found\r\nDate: ", HEADER_SIZE);
		break;
	case 0:
	default:
		strlcat(header, "500 Internal Server Error\r\nDate: ", HEADER_SIZE);
		break;
	}

	char datestring[DATE_SIZE];
	time_t rawtime;
	time(&rawtime);
	datetime(datestring, &rawtime);
	strlcat(header, datestring, HEADER_SIZE);
	strlcat(header, "\r\nServer: AdamFelix\r\nContent-Type: text/html\r\n\r\n", HEADER_SIZE);
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
