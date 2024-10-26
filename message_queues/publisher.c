#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <string.h>
#include <errno.h>

#define QUEUE_NAME "/test_queue"
#define MAX_MSG_SIZE        256
#define MSG_BUFFER_SIZE     (MAX_MSG_SIZE + 10)


int main(int argc, char **argv){
    char buffer[MSG_BUFFER_SIZE];
    int recvr_msgq_fd = 0;

    if ((recvr_msgq_fd  = mq_open (QUEUE_NAME, O_WRONLY | O_CREAT, 0, 0)) == -1) {
            printf ("Client: mq_open failed, errno = %d", errno);
            exit (1);
    }

    while (1)
    {
        memset(buffer, 0, MSG_BUFFER_SIZE);
        printf("Enter msg to be sent to subscriber %s\n", QUEUE_NAME);
        scanf("%s", buffer);

        if (mq_send(recvr_msgq_fd, buffer, strlen(buffer) + 1, 0) == -1) {
            perror ("Client: Not able to send message to server");
            exit (1);
        }
    }

    mq_close(recvr_msgq_fd);
    return 0;
}