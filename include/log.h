#ifndef HTTP_H
#define HTTP_H
#include <stdlib.h>
#include "structs.h"

/**
 * @file log.h
 * @brief functions that log requests to the server
 */

/**
 * @brief This function will be called from a thread to construct a log entry
 */
void writeToLog(char * filepath, HTTP_buffer * HTTP, int socketfd);
/**
 * @brief Writing to log file in a thread safe manner
 */
void writeToFile(char * filepath, char * buffer, int buffersize);
/**
 * @brief Fills the buffer with the clients IP-address
 */
void resolveIp(int socketfd, char * buffer);

#endif
