#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "HTTP.h"

int HTTP_Request(char *request, char **response, char *buffer, int buffersize, int offset) {
//TODO
//Switch case
//  GET
//  HEAD
//  POST/PUT/etc: not implemented
//  default: bad request
//return 0 or 1
    return GET(request,buffer,buffersize,offset);
}

int GET(char *request, char *buffer, int buffersize, int offset) {
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

int HEAD(char *request, char *buffer, int buffersize, int offset) {
    createHeader(buffer, 0);

    return 0;
}

int readFile(char *filepath, char *content) {
    //TODO check permission, file existence
    int statusCode;
    FILE *file = fopen(filepath, "r");
 
    if(file == NULL) {
	statusCode = 500;
    }  else {
	char ch;
	int i = 0;
	while((ch = fgetc(file)) != EOF && i < (FILE_SIZE-1))
	    content[i++] = ch;
	content[i] = '\0';
	fclose(file);
	statusCode = 200;
    }
    return statusCode;
}

int extractFilename(char *request, char *filepath) {
    int statusCode = 200;
    int namelen = PATH_SIZE - strlen(WWW);
    char *filename = calloc(namelen, sizeof(char));
    
    int i = 4; //Works for GET but not POST
    int j = 0;
    while(request[i] != ' ' && request[i] != '\n' && request[i] != '\r' && request[i] != '\\' && j < namelen - 1) //TODO Bryt ut till funktion validURLchar(request[i])
	filename[j++] = request[i++];
    filename[j] = '\0';
    

    if(filename[strlen(filename)-1] == '/')
	strcat(filename, "index.html"); //possible overflow
    if(request[i] == '\\' || request[i] == '\n' || request[i] == '\r')
	statusCode = 501;
    
    strcpy(filepath, WWW);
    strcat(filepath, filename);
    free(filename);

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
