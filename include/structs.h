#ifndef STRUCTS_H
#define STRUCTS_H

/**
 * @file structs.h
 * @brief All structs used are defined here
 */

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



#endif 
