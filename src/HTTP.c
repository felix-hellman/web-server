#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "HTTP.h"

extern int errno;

int HTTP_Request(HTTP_buffer *HTTP)
{
//TODO
//Switch case
//  GET
//  HEAD
//  POST/PUT/etc: not implemented
//  default: bad request
//return 0(done) or 1(continue)
	return GET(HTTP->request, HTTP->buffer, HTTP->buffersize, HTTP->offset);
}

//TODO handle those statuscodes
int GET(char *request, char *buffer, int buffersize, int offset)
{
	char filepath[PATH_SIZE];
	extractFilename(request, filepath);

	char content[FILE_SIZE];
	readFile(filepath, content);

	char response[HEADER_SIZE + FILE_SIZE];
	int length = strlen(content);
	createHeader(response, length);
	strcat(response, content);
	memcpy(buffer, &response[buffersize * offset], buffersize);

	if (strlen(response) < (offset+1)*buffersize)
		return 0;
	else
		return 1;
}

int HEAD(char *request, char *buffer, int buffersize, int offset)
{
	createHeader(buffer, 0);
	return 0;
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
	while (request[i] != ' ' && request[i] != '\n' && request[i] != '\r' && request[i] != '\\' && j < namelen - 1) //TODO Bryt ut till funktion validURLchar(request[i])
		filename[j++] = request[i++];
	filename[j] = '\0';

	if (filename[strlen(filename)-1] == '/')
		strcat(filename, "index.html"); //possible overflow
	strcpy(filepath, WWW);
	strcat(filepath, filename);
	free(filename);

	if (request[i] == '\\' || request[i] == '\n' || request[i] == '\r')
		return 501;
	return 200;
}

//TODO correct date, error code
void createHeader(char *header, int length)
{
	strcpy(header, "HTTP/1.0 200 OK\nDate: Tue, 12 Sep 2017 19:49:32 GMT\nServer: AdamFelix\nContent-Length: ");
	char lengthstr[12];
	sprintf(lengthstr, "%d", length);
	strcat(header, lengthstr);
	strcat(header, "\nContent-Type: text/html\n\n");
}
