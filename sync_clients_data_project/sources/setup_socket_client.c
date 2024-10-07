#include "setup_socket_client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int printMessages(int fd, const char *succ_msg, const char *err_msg) {
    if (fd == -1) {
        perror(err_msg);
        exit(EXIT_FAILURE);
    } else {
        printf("%s\n", succ_msg);
    }
    return fd;  // Return fd for further use if needed
}

int createClientSocket() {
    int client_fd = socket(AF_LOCAL, SOCK_STREAM, 0);
    printMessages(client_fd, "Client socket created", "Failed to create client socket");
    return client_fd;
}

int connectToServer(int client_fd) {
    struct sockaddr_un server_address;
 
    memset(&server_address, 0, sizeof(struct sockaddr_un));
    server_address.sun_family = AF_LOCAL;
    strncpy(server_address.sun_path, SOCKET_NAME, sizeof(server_address.sun_path) - 1); // Null byte

    return printMessages(connect(client_fd, (struct sockaddr *) &server_address, sizeof(server_address)),
                 "Connected to server", "Failed to connect to server");
                 
}

int readFromServer(int client_fd, char *buffer, unsigned int buffer_size) {
    int bytes_read = read(client_fd, buffer, buffer_size);
    printMessages(bytes_read, "Data read from server", "Failed to read data from server");
    if(bytes_read == 0) {
        perror("Faild to read data");
        exit(EXIT_FAILURE);
    }
    return bytes_read;
}

int writeToServer(int client_fd, char *buffer, unsigned int buffer_size) {
    int bytes_written = write(client_fd, buffer, buffer_size);
    printMessages(bytes_written, "Data sent to server", "Failed to send data to server");
    return bytes_written;
}

void closeClientSocket(int client_fd) {
    close(client_fd);
    printf("Client socket closed\n");
}
