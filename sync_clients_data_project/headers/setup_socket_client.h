#ifndef SETUP_SOCKET_CLIENT_H
#define SETUP_SOCKET_CLIENT_H

#include <sys/socket.h>
#include <sys/un.h>

#define  SOCKET_NAME "/tmp/SERVER_SOCKET"

int createClientSocket();
int printMessages(int fd, const char *succ_msg, const char *err_msg);
int connectToServer(int client_fd);
int readFromServer(int client_fd, char *buffer, unsigned int buffer_size);
int writeToServer(int client_fd, char *buffer, unsigned int buffer_size);
void closeClientSocket(int client_fd);

#endif // SETUP_SOCKET_CLIENT_H
