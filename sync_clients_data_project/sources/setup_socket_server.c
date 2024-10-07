#include "setup_socket_server.h"
#include "sample_encyrption.h"


int check(int fd, char* succ_msg, char* err_msg) {
    if(fd == -1) {
        printf("%s\n", err_msg);
        return -1;
    }
    printf("%s\n", succ_msg);
    return 0;
}

int createSocketServer() {
    unlink(SOCKET_NAME);

    int server_fd = socket(AF_LOCAL, SOCK_STREAM, 0);

    check(server_fd, "Server socket created", "Faild to create socket server");

    // bindSocketServer(server_fd);
    // setServerQueueSize(server_fd, QUEUE_SIZE);

    return server_fd;
}

int bindSocketServer(int server_fd) {
    struct sockaddr_un details;

    unsigned int struct_size = sizeof(struct sockaddr_un);

    memset(&details, 0, struct_size);

    details.sun_family = AF_LOCAL;
    strncpy(details.sun_path, SOCKET_NAME, sizeof(details.sun_path) - 1); //null byte

    return check( bind(server_fd,(const struct sockaddr *) &details, struct_size),
           "Binded the server socket",
           "Faild to bind the server socket");
}

int setServerQueueSize(int server_fd, int size) {
    return check( listen(server_fd, size),
           "Server queue size is set",
           "Faild to set server queue size");
}

int acceptNewClient(int server_fd) {
    int client_fd = accept(server_fd, NULL, NULL); // To use socket server details
    check(client_fd, "New Connection accepted", "Faild to get connection");
    return client_fd;
}

int readFromClient(int client_fd, char *buffer, unsigned int buffer_size) {
    return check(read(client_fd, buffer, buffer_size),
           "Data is read and saved inside buffer",
           "Faild to read data");
}

int writeToClient(int client_fd, char *buffer, unsigned int buffer_size) {
    encryptData(buffer, buffer_size);
    return check(write(client_fd, buffer, buffer_size),
           "Sending data to client",
           "Faild to send data to client");
}

int initServer() {
    int server_fd = createSocketServer();
    bindSocketServer(server_fd);
    setServerQueueSize(server_fd, MAX_NO_CLIENT);

    return server_fd;
}
