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
};
struct HTTP_buffer
{
  char *client_message;///< HTTP-request from the client
  char *response;///< HTTP-response from the server in it's entirety
  char *buffer;///< the response is split and buffered to the socket
  int buffersize;///< size of the buffer
  int offset;///< offset to keep track of what have been buffered
};
struct HTTP_request
{
	int method;///< 1=GET 2=HEAD -1=not implemented -2=bad request -3=forbidden -4=not found 0=internal server error
	char path[PATH_MAX];///< the requested resolved URL. 
	char raw_path[PATH_MAX];///< the requested URL
	int version;///< HTTP version number 9, 10 or 11 for HTTP/[0.9|1.0|1.1]
};

#endif
