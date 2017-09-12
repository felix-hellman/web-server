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

struct settingsdata
{
  int listeningport;
  char * filepath;
  char requestHandlingMode; //Should only be T for now (T = THREADS)
  int daemonMode; // 1 for yes, 0 for no
};
struct thread_data
{
 int working;
 int clientsocket;
};


void handleConnection(struct thread_data * data);
void spawn_connection(pthread_t * thread, struct thread_data * data);
void printUsage(char ** argv);
void threadCleanup(pthread_t *threads);
void handleArguments(struct settingsdata * settings, int argc, char ** argv);







#endif
