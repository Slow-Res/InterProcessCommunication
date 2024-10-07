#include "routing_table.h"
#include "setup_socket_server.h"
#include "monitor_fd_set.h"

Message* table[20] = { NULL };

// Function to get the maximum ID from the table
int getMaxId() {
    int max = 0;
    for (int i = 0; i < 20; i++) {
        if (table[i] != NULL && table[i]->id > max) {
            max = table[i]->id;
        }
    }
    return max;
}

// Function to insert a message into the table (CREATE operation)
int insertMessageToTable(Message *msg) {
    if(msg->id == -1) { // CREATE
        msg->id = getMaxId() + 1;
    }

    for (int i = 0; i < 20; i++) {
        if (table[i] == NULL) {
            table[i] = (Message*)malloc(sizeof(Message));  // Dynamically allocate memory
            if (table[i] == NULL) return -1;  // Memory allocation failure
            *table[i] = *msg;  // Copy the message to the allocated memory
            return 0;
        }
    }
    return -1;  // Table is full
}


// Function to update a message in the table (UPDATE operation)
int updateMessageInTable(Message *msg) {
    for (int i = 0; i < 20; i++) {
        if (table[i] != NULL && table[i]->id == msg->id) {
            *table[i] = *msg;  // Update the existing message
            return 0;
        }
    }
    return -1;  // No message with the given ID
}

// Function to delete a message from the table (DELETE operation)
int deleteMessageFromTable(Message *msg) {
    for (int i = 0; i < 20; i++) {
        if (table[i] != NULL && table[i]->id == msg->id) {
            free(table[i]);  // Free the memory
            table[i] = NULL;  // Set the entry to NULL
            return 0;
        }
    }
    return -1;  // No message with the given ID
}

void printTable() {
    printf("ID\t%-15s\tMask\t%-15s\t%s\n", "Destination", "Gateway IP", "OIF");
    printf("-------------------------------------------------------------\n");

    for (int i = 0; i < 20; i++) {
        if (table[i] != NULL) {  // Check if the slot is not empty
            printf("%d\t%-15s\t%d\t%-15s\t%s\n", 
                   table[i]->id, 
                   table[i]->destination, 
                   table[i]->mask, 
                   table[i]->gateway_ip, 
                   table[i]->oif);
        }
    }
    printf("\n");
}

Event parseMessage(char *buffer) {
    Event event;
    char *token;
    int field = 0;
    
    // Initialize event data
    memset(&event, 0, sizeof(Event));

    // Define attribute pointers and sizes
    void *attributes[] = {
        &event.data.id,
        event.data.destination,
        &event.data.mask,
        event.data.gateway_ip,
        event.data.oif
    };
    
    int sizes[] = {
        sizeof(int),
        sizeof(event.data.destination),
        sizeof(int),
        sizeof(event.data.gateway_ip),
        sizeof(event.data.oif)
    };

    // Parse the method
    token = strtok(buffer, ";");
    if (token) {
        event.method = (Method)atoi(token);
    }

    // Parse the remaining fields
    for (field = 0; field < 5; field++) {
        if (event.method == CREATE && field == 0) {
            *(int *) attributes[field] = -1;
            continue;
        }
        token = strtok(NULL, ";");
        if (token == NULL) break;

        if (sizes[field] == sizeof(int)) {
            *(int *)attributes[field] = atoi(token);
        } else {
            strncpy((char *)attributes[field], token, sizes[field] - 1);
            ((char *)attributes[field])[sizes[field] - 1] = '\0';
        }
    }

    // Process the event based on its method
    switch (event.method) {
        case CREATE:
            printf("Create event\n");
            insertMessageToTable(&event.data);
            break;
        case UPDATE:
            printf("Update event for id %d\n", event.data.id);
            updateMessageInTable(&event.data);
            break;
        case DELETE:
            printf("Delete event for id %d\n", event.data.id);
            deleteMessageFromTable(&event.data);
            break;
        default:
            printf("UNKNOWN EVENT\n");
            break;
    }

    return event;
}


void processEvent(Event *event) {
    if (event == NULL) {
        printf("Received NULL event\n");
        return;
    }

    // Process the event based on its method
    switch (event->method) {
        case CREATE:
            printf("Create event\n");
            insertMessageToTable(&event->data);
            break;
        case UPDATE:
            printf("Update event for id %d\n", event->data.id);
            updateMessageInTable(&event->data);
            break;
        case DELETE:
            printf("Delete event for id %d\n", event->data.id);
            deleteMessageFromTable(&event->data);
            break;
        default:
            printf("UNKNOWN EVENT\n");
            break;
    }
}

int dumpTableToNewClient(int new_client_fd, char *buffer) {
    printf("DUMPING TABLE TO NEW CLIENT\n");
    
    int i = 0;
    Event event;
    event.method = CREATE;
    for(; i < 20; i++) {
        if(table[i] != NULL) {
            event.data = *table[i];
            memset(buffer, 0, sizeof(event));
            memcpy(buffer, &event, sizeof(event));
            writeToClient(new_client_fd, buffer, sizeof(event));
        }
    }
    
    
    return 0;
}


// Function to broadcast events to all connected clients
int broadcastEvent(Event event, int *exclude_list, int exclude_list_size) {
    int num_clients;
    int *connected_clients = getAllConnectedClients(exclude_list, exclude_list_size, &num_clients);


    if(num_clients == 0) {
        printf("No Connected clients ATM");
        return 0;
    }

    char buffer[sizeof(event)];
    for (int i = 0; i < num_clients; i++) {
        int client_fd = connected_clients[i];
        memset(buffer, 0, sizeof(event));
        memcpy(buffer, &event, sizeof(event));

        writeToClient(client_fd, buffer, sizeof(event));

        printf("Broadcasting event to client fd: %d\n", client_fd);
    }

    return 0;
}


