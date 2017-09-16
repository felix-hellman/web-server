#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "HTTP.h"

extern int errno;

int HTTP_Request(struct HTTP_buffer *HTTP)
{
	HTTP->response = calloc((HEADER_SIZE + FILE_SIZE), sizeof(char));
	char method[8]; //The longest methods are 7 characthers() eg. CONNECT, OPTIONS
	int i = 0;
	while (HTTP->request[i] != ' ' && i < 7 && i < strlen(HTTP->request)) {
		method[i] = HTTP->request[i];
		i++;
	}
	method[i] = '\0';

	if (strcmp(method, "GET") == 0)
		return GET(HTTP);
	else if (strcmp(method, "HEAD") == 0)
		return HEAD(HTTP);
	else if (cmpNotImpl(method))
		return NOT_IMPL(HTTP);
	else
		return BAD_REQ(HTTP);
}

int GET(struct HTTP_buffer *HTTP)
{
	char filepath[PATH_SIZE];
	int statusCode;
	statusCode = extractFilename(HTTP->request, filepath);

	char content[FILE_SIZE] = "";
	if (statusCode == 200)
		statusCode = readFile(filepath, content);

	int length = strlen(content);
	createHeader(HTTP->response, length, statusCode);
	strcat(HTTP->response, content);

	return sendBuffer(HTTP);
}


int HEAD(struct HTTP_buffer *HTTP)
{
	createHeader(HTTP->response, 0, 200);
	return sendBuffer(HTTP);
}

int NOT_IMPL(struct HTTP_buffer *HTTP)
{
	createHeader(HTTP->response, 0, 501);
	return sendBuffer(HTTP);
}

int BAD_REQ(struct HTTP_buffer *HTTP)
{
	createHeader(HTTP->response, 0, 400);
	return sendBuffer(HTTP);
}

int sendBuffer(struct HTTP_buffer *HTTP)
{
	memcpy(HTTP->buffer, &HTTP->response[HTTP->buffersize * HTTP->offset], HTTP->buffersize);
	if (strlen(HTTP->response) > (HTTP->offset+2)*HTTP->buffersize) {
		return 1;
	} else {
		free(HTTP->response);
		return 0;
	}
}

int readFile(char *filepath, char *content)
{
	FILE *file = fopen(filepath, "r");

	if (file == NULL) {
		int errnum = errno;
		if (strcmp(strerror(errnum), "No such file or directory") == 0)
			return 404;
		else
			return 500;
	}  else {
		char ch;
		int i = 0;
		while ((ch = fgetc(file)) != EOF && i < (FILE_SIZE-1))
			content[i++] = ch;
		content[i] = '\0';
		fclose(file);
		return 200;
	}
}

//TODO kolla om adressen ligger på tillåten plats. Annars blir det 403! realpath
int extractFilename(char *request, char *filepath)
{
	int namelen = PATH_SIZE - strlen(WWW);
	char *filename = calloc(namelen, sizeof(char));

	int i = 4; //Works for GET but not POST
	int j = 0;
	while (request[i] != ' ' && !illegalURLchar(request[i]) && j < namelen - 12) //-12 to make room for possible index.html
		filename[j++] = request[i++];
	filename[j] = '\0';

	if (filename[strlen(filename)-1] == '/')
		strcat(filename, "index.html");
	strcpy(filepath, WWW);
	strcat(filepath, filename);
	free(filename);

	char resolved[PATH_SIZE];
	char *res = realpath(filepath, resolved);
	if (res == NULL)
	{
		return 500;
	}
	strcpy(filepath, resolved);

	i = 0;
	while (i < strlen(WWW) && i < strlen(filepath)) {
		if (WWW[i] != filepath[i])
			return 403;
		i++;
	}

	if (illegalURLchar(request[i]))
		return 501;
	return 200;
}

//TODO correct date
void createHeader(char *header, int length, int code)
{
	strcpy(header, "HTTP/1.0 ");
	char statusCode[30];
	switch (code) {
	case 200:
		strcpy(statusCode, "200 OK \n");
		break;
	case 400:
		strcpy(statusCode, "400 Bad Request \n");
		break;
	case 403:
		strcpy(statusCode, "403 Forbidden \n");
		break;
	case 404:
		strcpy(statusCode, "404 Not Found \n");
		break;
	case 501:
		strcpy(statusCode, "501 Not Implemented \n");
		break;
	case 500:
	default:
		strcpy(statusCode, "500 Internal Server Error \n");
		break;
	}
	strcat(header, statusCode);
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

int illegalURLchar(char ch)
{
	return (ch == '\\' || ch == '\n' || ch == '\r');
}
