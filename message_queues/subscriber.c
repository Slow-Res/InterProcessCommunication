#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <errno.h>
#include <sys/select.h>

#define MAX_MESSAGES 10
#define MAX_MSG_SIZE 256
#define MSG_BUFFER_SIZE     (MAX_MSG_SIZE + 10)
#define QUEUE_PERMISSIONS   0660
#define QUEUE_NAME "/test_queue"

/* 
Flags
O_RDONLY  only read from thequeue
O_WRONLY  only write to the queue
O_RDWR   read/write
O_CREAT  create if not exists

mode
 its set by the owning process usually 0660
*/

int main(int argc, char **argv){

    fd_set readfds;
    char buffer[MSG_BUFFER_SIZE];
    int msgq_fd = 0;

    /*To set msgQ attributes*/
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MESSAGES;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    if ((msgq_fd  = mq_open (QUEUE_NAME, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr)) == -1) {
        printf ("Client: mq_open failed, errno = %d", errno);
        exit (1);
    }

    FD_ZERO(&readfds); // Reset file descriptors set
    FD_SET(0, &readfds);
    FD_SET(msgq_fd, &readfds); // Add the msgQ fd to the set
    int message_read_count = 0;
    while(1){
        printf("Reciever blocked on select()....\n");
        select(msgq_fd + 1, &readfds, NULL, NULL, NULL); // Wait for events
        if(FD_ISSET(msgq_fd, &readfds)){
            printf("Msg recvd msgQ %s\n", QUEUE_NAME);
            message_read_count++;    
            memset(buffer, 0, MSG_BUFFER_SIZE);
            if (mq_receive (msgq_fd, buffer, MSG_BUFFER_SIZE, NULL) == -1) {
                printf ("mq_receive error, errno = %d\n", errno);
                exit (1);
            }
            printf("Msg recieved from Queue = %s\n", buffer);
            FD_SET(msgq_fd, &readfds);
        }

        if(FD_ISSET(0, &readfds)) // Print message read count if STDIN triggerd
        {
            printf("Total read messages: %d\n", message_read_count);
            FD_SET(0, &readfds);
            getchar(); // Consume the newline character to reset stdin detection
        }
    }
}