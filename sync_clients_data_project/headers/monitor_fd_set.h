#ifndef MONITOR_FD_SET_H
#define MONITOR_FD_SET_H

#include <stdio.h>
#include <sys/select.h>  // For fd_set operations
#include <unistd.h>      // For close()
#include <stdbool.h>

// Constants
#ifndef MAX_NO_CLIENTS
    #define MAX_NO_CLIENTS 50
#endif

// Global variables
// FDs array which server process is maintaining at once
extern int monitored_fd_set[MAX_NO_CLIENTS];

// Function declarations
void initializeMonitorFdSet();
void addToMonitoredFdSet(int skt_fd);
void removeFromMonitoredFdSet(int skt_fd);
int getMaxFd();
void refreshFdSet(fd_set *fd_set_ptr);
int findTriggeredClientFdIndex(fd_set *fd_set_ptr);
void printMonitoredFds();
bool isFdInExcludeList(int fd, int *exclude_list, int exclude_list_size);
int* getAllConnectedClients(int *exclude_list, int exclude_list_size, int *num_clients);

#endif // MONITOR_FD_SET_H
