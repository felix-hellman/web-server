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
void writeToLog(char * filepath, struct HTTP_buffer * HTTP, char * ipaddress);
/**
 * @brief Writing to log file in a thread safe manner
 */
void writeToFile(char * filepath, char * buffer, int buffersize);
/**
 * @brief Concatenates a string to put in the log file
 */
void logcat(char *entry, struct log_entry *le);


#endif
