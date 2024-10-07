#include "setup_socket_client.h"
#include "routing_table.h"
#include "sample_encyrption.h"


#define BUFFER_SIZE 50 * 128
char buffer[BUFFER_SIZE];


void clientLoop(int client_fd) {
    Event *received_event;
    while(1) {
        memset(buffer, 0, BUFFER_SIZE);
        printf("Waiting for server to send data\n");
        readFromServer(client_fd, buffer, sizeof(Event));
        decryptData(buffer, BUFFER_SIZE);
        received_event = (Event *) buffer;
        printf("Received Event from server: %d\n", received_event->method);
        processEvent(received_event);
        printTable();
    }
}

int main() {
    int client_fd = createClientSocket();
    connectToServer(client_fd);
    clientLoop(client_fd);

    closeClientSocket(client_fd);
    return 0;
}