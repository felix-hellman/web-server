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
    return GET(client_message,buffer,buffersize,offset);
}

int GET(char * client_message, char * buffer, int buffersize,int offset) {
    //char *filepath = extractFilepath(client_message);
    char content[MAX_FSIZE];
    FILE *file = fopen("/var/www/index.html", "r");
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

int HEAD(char * client_message, char * buffer, int buffersize,int offset) {
    createHeader(buffer, 0);
    
    return 0;
}

char **extractFilepath(char * request) {
    /*char filename[100];
    int i = 4;
    int j = 0;
    while(request[i] != ' ' && j < 99)
	filename[j++] = request[i++];
    filename[j] = '\0';
    
    char fullpath[120];
    strcpy(fullpath, WWW);
    strcat(fullpath, filename);
    
    return fullpath;*/
    return NULL;
}

//TODO correct date, error code
void createHeader(char *header, int length) {
    strcpy(header, "HTTP/1.0 200 OK\nDate: Tue, 12 Sep 2017 19:49:32 GMT\nServer: AdamFelix\nContent-Length: ");
    char lengthstr[12];
    sprintf(lengthstr, "%d", length);
    strcat(header, lengthstr);
    strcat(header, "\nContent-Type: text/html\n\n");
}
