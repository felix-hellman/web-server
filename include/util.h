#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "HTTP.h"
#include "structs.h"
#include <sys/time.h>

/**
 * @file util.h
 * @brief Utility functions for the webserver backend
 */



void handleConnection(int socketfd);

/**
 * @brief Handles connections, serves requests and closes sockets when done
 */
void threadHandleConnection(struct thread_data * data);
/**
 * @brief Calls the handleConnection in a separate thread
 */
void init_thread(pthread_t * thread, struct thread_data * data);
/**
 * @brief Prints the help text on how to use the program
 */
void printUsage(char ** argv);
/**
 * @brief Joins threads
 */
void threadCleanup(pthread_t *threads, int nr);
/**
 * @brief Handles input arguments from commandline
 * @param settings a pointer to a struct settingsdata to be filled
 * @param argc number of arguments
 * @param argv vector of arguments
 */
void handleArguments(struct settingsdata * settings,struct configsettings * defaultsettings, int argc, char ** argv);







#endif
