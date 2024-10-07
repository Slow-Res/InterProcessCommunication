#ifndef SETUP_SOCKET_SERVER_H
#define SETUP_SOCKET_SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>  // For write() system call


#ifndef SOCKET_NAME
    #define SOCKET_NAME "/tmp/SERVER_SOCKET"
#endif

#ifndef MAX_NO_CLIENT
    #define MAX_NO_CLIENT 50
#endif

// Function declarations
int check(int fd, char* succ_msg, char* err_msg);
int createSocketServer();
int bindSocketServer(int server_fd);
int setServerQueueSize(int server_fd, int size);
int acceptNewClient(int server_fd);
int readFromClient(int client_fd, char *buffer, unsigned int buffer_size);
int writeToClient(int client_fd, char *buffer, unsigned int buffer_size);
int initServer();

#endif // SETUP_SOCKET_SERVER_H
