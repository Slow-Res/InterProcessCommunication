#ifndef ROUTING_TABLE_H
#define ROUTING_TABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Enum for operations
typedef enum {
    CREATE = 1,
    UPDATE = 2,
    DELETE = 3
} Method;

// Struct for a message (routing table row)
typedef struct Message {
    int id;
    char destination[16];
    int mask;
    char gateway_ip[16];
    char oif[32];  // Outgoing interface
} Message;

// Struct for Event
typedef struct Event {
    Method method;
    Message data;
} Event;

// Global table for storing messages (20 entries)
extern Message* table[20];

// Function prototypes
int insertMessageToTable(Message *msg);
int updateMessageInTable(Message *msg);
int deleteMessageFromTable(Message *msg);
void printTable();
Event parseMessage(char *buffer);
int getMaxId();
int broadcastEvent(Event event, int *exclude_list, int exclude_list_size);
int dumpTableToNewClient(int new_client_fd, char *buffer);
void processEvent(Event *event);


#endif // !ROUTING_TABLE_H