#include "monitor_fd_set.h"

int monitored_fd_set[MAX_NO_CLIENTS];

// Initialize the monitored_fd_set array by setting all entries to -1
void initializeMonitorFdSet() {
    for(int i = 0; i < MAX_NO_CLIENTS; i++) {
        monitored_fd_set[i] = -1;
    }
}

// Add a socket file descriptor to the monitored_fd_set array
void addToMonitoredFdSet(int skt_fd) {
    for(int i = 0; i < MAX_NO_CLIENTS; i++) {
        if(monitored_fd_set[i] == -1) {
           monitored_fd_set[i] = skt_fd;
           return;
        }
    }
}

// Remove a socket file descriptor from the monitored_fd_set array
void removeFromMonitoredFdSet(int skt_fd) {
    for(int i = 0; i < MAX_NO_CLIENTS; i++) {
        if (monitored_fd_set[i] == skt_fd) {
            monitored_fd_set[i] = -1;
            break;
        }
    }
}

// Find and return the maximum socket file descriptor in the monitored_fd_set array
int getMaxFd() {
    int max_fd = -1;
    for (int i = 0; i < MAX_NO_CLIENTS; i++) {
        if (monitored_fd_set[i] > max_fd) {
            max_fd = monitored_fd_set[i];
        }
    }
    return max_fd;
}

// Refresh the fd_set based on the currently monitored file descriptors
void refreshFdSet(fd_set *fd_set_ptr) {
    FD_ZERO(fd_set_ptr); // Resets the fd_set
    for(int i = 0; i < MAX_NO_CLIENTS; i++) {
        if(monitored_fd_set[i] != -1)
            FD_SET(monitored_fd_set[i], fd_set_ptr); // Adds FDs to the set
    }
}

// Find the index of the triggered file descriptor in the fd_set
int findTriggeredClientFdIndex(fd_set *fd_set_ptr) {
    for(int i = 0; i < MAX_NO_CLIENTS; i++) {
        if(FD_ISSET(monitored_fd_set[i], fd_set_ptr))
            return i;
    }
    return -1; // No triggered client found
}

// Print all the monitored file descriptors
void printMonitoredFds() {
    printf("Monitored FDs: ");
    for (int i = 0; i < MAX_NO_CLIENTS; i++) {
        if (monitored_fd_set[i] != -1) {
            printf("%d ", monitored_fd_set[i]);
        }
    }
    printf("\n");
}

// Function to check if a file descriptor is in the exclude list
bool isFdInExcludeList(int fd, int *exclude_list, int exclude_list_size) {
    for (int i = 0; i < exclude_list_size; i++) {
        if (fd == exclude_list[i]) {
            return true; // Found in exclude list
        }
    }
    return false; // Not found
}

// Function to get all connected client file descriptors excluding the ones in exclude_list
int* getAllConnectedClients(int *exclude_list, int exclude_list_size, int *num_clients) {
    static int connected_clients[MAX_NO_CLIENTS]; // Array to store connected clients
    int client_count = 0;

    // Loop through all monitored file descriptors
    for (int i = 0; i < MAX_NO_CLIENTS; i++) {
        int fd = monitored_fd_set[i];

        // If fd is valid (not -1) and not in the exclude list
        if (fd != -1 && !isFdInExcludeList(fd, exclude_list, exclude_list_size)) {
            connected_clients[client_count++] = fd;  // Add to result list
        }
    }

    *num_clients = client_count;  // Set the number of clients found
    return connected_clients;     // Return the array of connected clients
}



