
#include "monitor_fd_set.h"
#include "setup_socket_server.h"
#include "routing_table.h"

#define SOCKET_NAME "/tmp/SERVER_SOCKET"
#define BUFFER_SIZE 128 * 50
fd_set readfds;
char buffer[BUFFER_SIZE];

void serverLoop(int server_fd, int* exclude_list, int list_size) {    
    while(1) {
        refreshFdSet(&readfds);
        printf("Waiting on select() sys call for new events\n");
        select(getMaxFd() + 1, &readfds, NULL, NULL, NULL);

        if(FD_ISSET(server_fd, &readfds)) {
            int client_fd = acceptNewClient(server_fd);
            addToMonitoredFdSet(client_fd);
            dumpTableToNewClient(client_fd, buffer);
        }
        else if (FD_ISSET(0, &readfds)) {
            memset(buffer, 0, BUFFER_SIZE);
            read(0, buffer, BUFFER_SIZE); // Read from console
            Event event = parseMessage(buffer);
            
            broadcastEvent(event, exclude_list, list_size);
            printTable();
        }        
    }
}

int main() {
    initializeMonitorFdSet();

    int server_fd = initServer();
    addToMonitoredFdSet(0); // Add std_in 0 file descriptor to read data from console
    addToMonitoredFdSet(server_fd);

    int exclude_list[] = {0, server_fd};
    serverLoop(server_fd, exclude_list, 2);

    return 0;
}