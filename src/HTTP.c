#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "HTTP.h"

//TODO read filename, check if size > buffersize
int GET(char * client_message, char * buffer, int buffersize,int offset)
{
    char header[MAX_HSIZE];
    createHeader(header);
    char response[MAX_FSIZE + MAX_HSIZE];
    strcpy(response, header);

    FILE *file = fopen("index.html", "r");
    char ch;
    int i = strlen(header);
    while((ch = fgetc(file)) != EOF && i < MAX_FSIZE)
	response[i++] = ch;
    response[i] = '\0';
    fclose(file);
    
    memcpy(buffer, &response[buffersize * offset], buffersize);

    return 0;
}

int HEAD(char * client_message, char * buffer, int buffersize,int offset)
{
    createHeader(buffer);
    
    return 0;
}

//TODO our own header
void createHeader(char *header)
{
    strcpy(header, "HTTP/1.1 200 OK\nDate: Tue, 12 Sep 2017 19:49:32 GMT\nServer: Apache/2.4.10 (Raspbian)\nLast-Modified: Sun, 13 Nov 2016 21:54:02 GMT\nETag: \"c8-54135c2e00c0b\"\nAccept-Ranges: bytes\nContent-Length: 200\nVary: Accept-Encoding\nContent-Type: text/html\n");
}
