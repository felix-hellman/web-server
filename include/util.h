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



void handleConnection(int client_sock);
void printUsage(char ** argv);
void *printHello(int number); //This is mainly for debugging reasons @ pthread
void threadCleanup(pthread_t *threads);






#endif
