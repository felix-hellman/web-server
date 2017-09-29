#ifndef LOG_H
#define LOG_H
#include <stdlib.h>
#include "structs.h"

#define LOG_ENTRY_SIZE 1024

/**
 * @file log.h
 * @brief functions that log requests to the server
 */

/**
 * @brief This function will be called from a thread to construct a log entry
 */
void writeToLog(char * filepath, struct HTTP_buffer * HTTP, int socketfd);
/**
 * @brief Writing to log file in a thread safe manner
 */
void writeToFile(char * filepath, char * buffer, int buffersize);
/**
 * @brief Puts the clients ip-address in the log_entry struct
 */
void resolveIp(int socketfd, struct log_entry *le);
/**
 * @brief Concatenates a string to put in the log file
 */
void logcat(char *entry, struct log_entry *le);
/**
 * @brief Puts the content length in the log_entry struct
 */
void resolveBytes(struct log_entry *le, struct HTTP_buffer *HTTP);
/**
 * @brief Puts the HTTP request in the log_entry struct
 */
void resolveRequest(struct log_entry *le, struct HTTP_buffer *HTTP);
/**
 * @brief Puts the current date and time in the log_entry struct
 */
void resolveDate(struct log_entry *le, struct HTTP_buffer *HTTP);
/**
 * @brief Puts the response code in the log_entry struct
 */
void resolveCode(struct log_entry *le, struct HTTP_buffer *HTTP);

#endif
