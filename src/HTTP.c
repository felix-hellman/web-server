#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "HTTP.h"

int HTTP_Request(char *request, char **response, char *buffer, int buffersize, int offset) {
//Switch case
//  GET
//  HEAD
//  POST/PUT/etc: not implemented
//  default: bad request
//return 0 or 1
    return GET(request,buffer,buffersize,offset);
}

int GET(char *request, char *buffer, int buffersize, int offset) {
    char filepath[1024];
    int statusCode = extractFilepath(request, filepath);

    char content[MAX_FSIZE];
    FILE *file = fopen(filepath, "r");
    char ch;
    int i = 0;
    while((ch = fgetc(file)) != EOF && i < (MAX_FSIZE-1))
	content[i++] = ch;
    content[i] = '\0';
    fclose(file);
    
    char response[MAX_HSIZE + MAX_FSIZE];
    int length = strlen(content);
    createHeader(response, length);
    strcat(response, content);
    memcpy(buffer, &response[buffersize * offset], buffersize);

    if (strlen(response) < (offset+1)*buffersize)
	return 0;
    else
	return 1;
}

int HEAD(char *request, char *buffer, int buffersize, int offset) {
    createHeader(buffer, 0);

    return 0;
}

//TODO check permission, file existence
int extractFilepath(char *request, char *filepath) {
    int statusCode = 200;
    strcpy(filepath, WWW);
    int i = 4;
    int j = strlen(WWW);
    while(request[i] != ' ' && request[i] != '\n' && request[i] != '\r' && request[i] != '\\' && j < 1024) //Bryt ut till funktion validURLchar(request[i])
	filepath[j++] = request[i++];
    filepath[j] = '\0';
    
    if(request[i] == '\\' || request[i] == '\n' || request[i] == '\r')
	statusCode = 501;
    return statusCode;
}

//TODO correct date, error code
void createHeader(char *header, int length) {
    strcpy(header, "HTTP/1.0 200 OK\nDate: Tue, 12 Sep 2017 19:49:32 GMT\nServer: AdamFelix\nContent-Length: ");
    char lengthstr[12];
    sprintf(lengthstr, "%d", length);
    strcat(header, lengthstr);
    strcat(header, "\nContent-Type: text/html\n\n");
}
