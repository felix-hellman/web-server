#ifndef STRUCTS_H
#define STRUCTS_H

#include <limits.h>
/**
 * @file structs.h
 * @brief All structs used are defined here
 */
 struct configsettings
 {
 	int port;
 	char * rootdirectory;
 	char * requesthandling;
 };
struct settingsdata
{
  int listeningport;///< listening port for the program
  char * filepath; ///< filepath for log output
  char requestHandlingMode; ///<Should only be T for now (T = THREADS)
  int daemonMode; ///< 1 for yes, 0 for no
};
struct thread_data
{
 int working; ///< is 1 if working and 0 if done with handling a request
 int clientsocket; ///< client socket passed to the thread for processing a request
 int thread_id;
 char * WWW;
};
struct HTTP_buffer
{
	char *client_message;///< HTTP-request from the client
	char *response;///< HTTP-response from the server in it's entirety
	char *buffer;///< the response is split and buffered to the socket
	int buffersize;///< size of the buffer
	int offset;///< offset to keep track of what have been buffered
	int method;///< 1=GET 2=HEAD -1=not implemented -2=bad request -3=forbidden -4=not found 0=internal server error
	char raw_path[PATH_MAX];///< the requested URL
	char path[PATH_MAX];///< the requested resolved URL.
	int version;///< HTTP version number 9, 10 or 11 for HTTP/[0.9|1.0|1.1]
	char modified[37];///< last time modified for the requested file
	int content_type;///< contet-type fo requested file. 0=application/octet-stream, 1=text/html
	size_t content_length;///< size of the content in bytes
	int response_size;///< size of the entire response
  char * WWW;
};
struct log_entry
{
	char ip[20];///< client IP
	char date[28];///< date and time of the request
	char request[900];///< the HTTP request
	char code[4];///< response code
	char bytes[14];///< content size sent
};
#endif
