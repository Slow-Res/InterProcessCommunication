#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>

#define SOCKET_NAME "/tmp/SERVER_SOCKET"
#define BUFFER_SIZE 256
#define QUEUE_SIZE 20

/* MULTIPLEXING */

// FDs array which server process is maintaining at once
int monitored_fd_set[QUEUE_SIZE];
int client_result[QUEUE_SIZE] = {0};

static void initializeMonitorFdSet() {
    int i = 0;
    for(; i < QUEUE_SIZE; i++) {
        monitored_fd_set[i] = -1;
    }
}

static void addToMonitoredFdSet(int skt_fd) {
    int i = 0;
    for(; i < QUEUE_SIZE ; i++) {
        if(monitored_fd_set[i] == -1) {
           monitored_fd_set[i] = skt_fd;
           return;
        }
    }
}

static void removeFromMonitoredFdset(int skt_fd) {
    int i = 0;

    for(; i < QUEUE_SIZE; i++) {
        if ( monitored_fd_set[i] != skt_fd ) continue;
        monitored_fd_set[i] = -1;
        client_result[i] = 0;
        break;
    }
}

static int getMaxFd() {
    int i = 0;
    int max = -1;

    for (; i < QUEUE_SIZE ; i++) {
        if(monitored_fd_set[i] > max){
            max = monitored_fd_set[i];
        }
         
    }
    return max;
}

static void refreshFdSet(fd_set *fd_set_ptr) {
    FD_ZERO(fd_set_ptr); // Resets the fd_set
    int i = 0;
    for(; i < QUEUE_SIZE; i++) {
        if(monitored_fd_set[i] != -1)
            FD_SET(monitored_fd_set[i], fd_set_ptr); // Adds FDs to the set
    }
}

static int findTriggerdClientFdIndex(fd_set *fd_set_ptr) {
    int i = 0;
    for(; QUEUE_SIZE; i++) {
        if(FD_ISSET(monitored_fd_set[i], fd_set_ptr))
            return i;
    }
}

static void printMonitoredFds() {
    for (int i = 0; i < QUEUE_SIZE; i++) {
        if (monitored_fd_set[i] != -1) {
            printf("%d ", monitored_fd_set[i]);
        }
    }
}


/*****************/


void check(int fd, char* succ_msg, char* err_msg) {
    if(fd == -1) {
         printf("%s\n", err_msg);
        exit(EXIT_FAILURE);
    }
    printf("%s\n", succ_msg);
}

int createSocketServer() {
    unlink(SOCKET_NAME);

    int server_fd = socket(AF_LOCAL, SOCK_STREAM, 0);

    check(server_fd, "Server socket created", "Faild to create socket server");

    return server_fd;
}

void bindSocketServer(int server_fd) {
    struct sockaddr_un details;

    unsigned int struct_size = sizeof(struct sockaddr_un);

    memset(&details, 0, struct_size);

    details.sun_family = AF_LOCAL;
    strncpy(details.sun_path, SOCKET_NAME, sizeof(details.sun_path) - 1); //null byte

    check( bind(server_fd,(const struct sockaddr *) &details, struct_size),
           "Binded the server socket",
           "Faild to bind the server socket");
}

void setServerQueueSize(int server_fd, int size) {
    check( listen(server_fd, size),
           "Server queue size is set",
           "Faild to set server queue size");
    addToMonitoredFdSet(server_fd);
}

int getNextClient(int server_fd) {
    int client_fd = accept(server_fd, NULL, NULL); // To use socket server details
    check(client_fd, "New Connection accepted", "Faild to get connection");

    addToMonitoredFdSet(client_fd);

    return client_fd;
}

void readFromClient(int client_fd, char *buffer) {
    printf("Waiting for data from the client\n");

    check(read(client_fd, buffer, BUFFER_SIZE),
          "Data is set to buffer",
          "Faild to read data");
}

void writeToClient(int client_fd, char *buffer) {
    check(write(client_fd, buffer, BUFFER_SIZE),
          "Sending data to client",
          "Faild to send data to client");
}

void handleClient(int client_idx, char *buffer) {
    int receivedInt;
    int client_fd = monitored_fd_set[client_idx];

    memset(buffer, 0, BUFFER_SIZE);
    readFromClient(client_fd, buffer);
    memcpy(&receivedInt, buffer, sizeof(int)); // Turning data into int as expected

    if(receivedInt == 0) { // Terminating signal
        memset(buffer, 0, BUFFER_SIZE);
        snprintf(buffer, BUFFER_SIZE, "Result of sum = %d", client_result[client_idx]);
        writeToClient(client_fd, buffer);
        removeFromMonitoredFdset(client_fd);
        close(client_fd);
    }

    client_result[client_idx] += receivedInt;

}

void serverLoop(int server_fd) {
    char buffer[BUFFER_SIZE];
    fd_set readfds;
    while(1) {
        refreshFdSet(&readfds); /*Copy the entire monitored FDs to readfds*/

        /* Wait for incoming connection. */
        printf("Waiting on select() sys call\n");

        /* Call the select system call, server process blocks here. 
         * Linux OS keeps this process blocked untill the connection initiation request Or 
         * data requests arrives on any of the file Drscriptors in the 'readfds' set*/
        select(getMaxFd() + 1, &readfds, NULL, NULL, NULL);


        printf("NEW EVENT OCCURR\n");

        if(FD_ISSET(server_fd, &readfds)) // New connection triggers server_fd
            getNextClient(server_fd);
        else if (FD_ISSET(0, &readfds)) { // Console input triggers 0 fd
            memset(buffer, 0, BUFFER_SIZE);
            read(0, buffer, BUFFER_SIZE);
            printf("Echo back user input %s\n", buffer);
        }
        else { // Client sent data
            int client_idx = findTriggerdClientFdIndex(&readfds);
            handleClient(client_idx, buffer);
        }
        
    }
}

int main() {
    initializeMonitorFdSet();
    int server_fd = createSocketServer();
    bindSocketServer(server_fd);
    setServerQueueSize(server_fd, QUEUE_SIZE);
    addToMonitoredFdSet(0); // Add std_in 0 file descriptor to read data from console
    serverLoop(server_fd);

    return 0;
}