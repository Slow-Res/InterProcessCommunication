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

int createServerSocket() {
    /*
        int unlink(const char *pathname);

        - pathname: Path to the file (in this case, the socket file).
        
        This removes an existing socket file, if any, to avoid conflicts when creating a new one.
        Example: unlink(SOCKET_NAME)
    */
    unlink(SOCKET_NAME);  // Remove existing socket file if it exists

    /*
        int socket(int domain, int type, int protocol);

        domain:
            AF_UNIX OR AF_LOCAL  Local communication 
            AF_INET              IPv4 Internet protocols 
            AF_INET6             IPv6 Internet protocols
            AF_BLUETOOTH         Bluetooth low-level socket protocol

        type:
            SOCK_STREAM          Used with TCP (Transmission Control Protocol)
                                 Two-way, connection-based byte streams
       
            SOCK_DGRAM           Used with UDP (User Datagram Protocol)
                                 Supports datagrams (connectionless, unreliable messages of a fixed maximum length)

            SOCK_RAW             Provides raw network protocol access   


       protocol:
            0                    For default use for the selected domain and type
            IPPROTO_TCP (6)      Use cases: Web browsing (HTTP), email (SMTP), file transfer (FTP)   
            IPPROTO_UDP (17)     Use cases: DNS queries, streaming media, online gaming
            IPPROTO_ICMP (1)     Used for IP operation control and error reporting (ping, traceroute)
            BTPROTO_RFCOMM       Used for Bluetooth communication

    */

    int serverSocket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("socket creation failed");  // Prints the last error encountered
        exit(EXIT_FAILURE);
    }

    printf("Server socket created successfully\n");
    return serverSocket;
}

void bindServerSocket(int serverSocket) {
    struct sockaddr_un name;
    memset(&name, 0, sizeof(struct sockaddr_un)); // Initialize

    name.sun_family = AF_UNIX;  // Specify the communication domain (UNIX)
    strncpy(name.sun_path, SOCKET_NAME, sizeof(name.sun_path) - 1);  // Set the socket file path

    /*
        int bind(int socket, const struct sockaddr *address, socklen_t address_len);

        - socket: The file descriptor of the socket to bind.
        - address: Pointer to a structure that contains the address to bind to (sockaddr_un in this case).
        - address_len: Size of the address structure.


        This associates the socket with the specified file path. Such that a sender process
        sends the data to socket path then it will be deliverd to this server process
    */

    if (bind(serverSocket, (const struct sockaddr *)&name, sizeof(struct sockaddr_un)) == -1) {
        perror("bind failed");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    printf("Binding the server socket to %s succeeded\n", SOCKET_NAME);
}

void setServerSocketBacklog(int serverSocket, int maximumSize) {
    /*
        int listen(int socket, int backlog);

        - socket: The server socket file descriptor.
        - backlog: The maximum number of pending connections in the queue.

        This prepares the server to accept incoming connections.
    */
    if (listen(serverSocket, maximumSize) == -1) {
        perror("listen failed");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }
}

int acceptClientConnection(int serverSocket) {
    printf("Waiting on accept() system call\n");

    /*
        int accept(int socket, struct sockaddr *restrict address, socklen_t *restrict address_len);

        - socket: The server socket file descriptor.
        - address: Optional sockaddr structure to store the client's address (if address is null it will se the socket address).
        - address_len: Optional size of the address structure (if address is NULL it will use the socekt address_len).

        This accepts an incoming connection and returns a new socket for communication with the client.
    */
    int clientSocket = accept(serverSocket, NULL, NULL);
    if (clientSocket == -1) {
        perror("accept failed");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    printf("Connection accepted from client\n");
    return clientSocket;
}

int readFromClient(int clientSocket, char *buffer) {
    printf("Waiting for data from the client\n");

    /*
        ssize_t read(int fd, void *buf, size_t count);

        - fd: The file descriptor to read from (client socket).
        - buf: Pointer to a buffer where the data will be stored.
        - count: The maximum number of bytes to read.

        This blocks until data is available from the client.
    */
    int bytesRead = read(clientSocket, buffer, BUFFER_SIZE);
    if (bytesRead == -1) {
        perror("read failed");
        close(clientSocket);
        return -1;
    }
    return bytesRead;
}

void writeToClient(int clientSocket, const char *buffer) {
    /*
        ssize_t write(int fd, const void *buf, size_t count);

        - fd: The file descriptor to write to (client socket).
        - buf: Pointer to the data to be written.
        - count: The number of bytes to write.

        This sends the response back to the client.
    */
    if (write(clientSocket, buffer, BUFFER_SIZE) == -1) {
        perror("write failed");
        close(clientSocket);
        exit(EXIT_FAILURE);
    }
}

void handleClient(int clientSocket) {
    char buffer[BUFFER_SIZE];
    int totalSum = 0;
    int receivedInt;

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);  // Clear the buffer before receiving data
        int bytesRead = readFromClient(clientSocket, buffer);
        if (bytesRead <= 0) {
            break;
        }
        memcpy(&receivedInt, buffer, sizeof(int));

        // If the received integer is 0, break the loop (indicates end of transmission)
        if (receivedInt == 0) {
            break;
        }

        totalSum += receivedInt;  // Accumulate the sum of the received integers
    }
    memset(buffer, 0, BUFFER_SIZE);

    // Prepare the result message to send back to the client
    snprintf(buffer, BUFFER_SIZE, "Result = %d", totalSum);

    printf("Sending final result back to client\n");
    writeToClient(clientSocket, buffer);
    close(clientSocket);
}

void serverLoop(int serverSocket) {
    while (1) {
        int clientSocket = acceptClientConnection(serverSocket);
        handleClient(clientSocket);
    }
}

int main() {
    int serverSocket = createServerSocket();
    bindServerSocket(serverSocket);
    setServerSocketBacklog(serverSocket, QUEUE_SIZE);
    serverLoop(serverSocket);
    close(serverSocket);

    return 0;
}