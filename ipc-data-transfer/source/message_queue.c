#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include "buffer.h"

#define MSG_KEY 12345  // Message queue key
#define MSG_TYPE 1
#define MSG_CAPACITY 8192

typedef struct 
{
    long msgType;
    char msgText[MSG_CAPACITY];
} msg;

// Sender function
int sender_message_queue(const char* inputFilename, size_t bufferCapacity) {
    // Create message queue
    int msgid = msgget(MSG_KEY, IPC_CREAT | 0666);
    if (msgid == -1) 
    {
        perror("msgget failed");
        return -1;
    }

    int fileDesc = OpenFile(inputFilename);
    if (fileDesc == -1) 
    {
        perror("Failed to open input file");
        return -1;
    }

    Buffer* buffer = CreateBuffer(bufferCapacity);
    msg msgBuffer = {.msgType = MSG_TYPE, .msgText = {}};

    int msgSize = 0;
    while ((msgSize = readFromFile(buffer, fileDesc)) > 0) 
    {
        size_t curPos = 0;

        // Send data in chunks of MSG_CAPACITY
        while (curPos < buffer->size) 
        {
            int chunkSize = (buffer->size - curPos > MSG_CAPACITY) ? MSG_CAPACITY : buffer->size - curPos;
            memcpy(msgBuffer.msgText, buffer->buffer + curPos, chunkSize);

            if (msgsnd(msgid, &msgBuffer, chunkSize, 0) == -1) 
            {
                perror("Message send error");
                return -1;
            }

            curPos += chunkSize;
        }
    }

    // Send termination message (0-size message)
    msgBuffer.msgType = MSG_TYPE;
    msgBuffer.msgText[0] = '\0'; // Empty message to signal termination
    if (msgsnd(msgid, &msgBuffer, 0, 0) == -1) 
    {
        perror("Failed to send termination message");
        return -1;
    }

    close(fileDesc);
    DestroyBuffer(buffer);

    printf("Parent: Data sent via message queue\n");
    return 0;
}

// Receiver function
int receiver_message_queue(const char* outputFilename)
{
    int msgid = msgget(MSG_KEY, 0666);
    if (msgid == -1) 
    {
        perror("msgget failed");
        return -1;
    }

    int fileDesc = CreateFile(outputFilename);
    msg msgBuffer = {};

    while (1) 
    {
        int msgSize = msgrcv(msgid, &msgBuffer, MSG_CAPACITY, MSG_TYPE, 0);
        if (msgSize == -1) 
        {
            perror("Message receive error");
            return -1;
        }

        // Check for termination message
        if (msgSize == 0) break;

        if (write(fileDesc, msgBuffer.msgText, msgSize) < 0) 
        {
            perror("Write error");
            return -1;
        }
    }

    close(fileDesc);

    struct msqid_ds qstatus;
    msgctl(msgid, IPC_RMID, &qstatus);

    printf("Child: Data received via message queue\n");
    return 0;
}

int main(int argc, char* argv[]) 
{
    if (argc != 2) 
    {
        fprintf(stderr, "Usage: %s <BUFFER_SIZE>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Parse the buffer size argument
    size_t bufferCapacity = 0;
    if (strcmp(argv[1], "SMALL") == 0)
    {
        bufferCapacity = SMALL_BUFFER_SIZE;
    }
    else if (strcmp(argv[1], "MEDIUM") == 0)
    {
        bufferCapacity = MEDIUM_BUFFER_SIZE;
    }
    else if (strcmp(argv[1], "LARGE") == 0) 
    {
        bufferCapacity = LARGE_BUFFER_SIZE;
    }
    else 
    {
        fprintf(stderr, "Invalid buffer size. Use SMALL, MEDIUM, or LARGE.\n");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();

    if (pid == -1) 
    {
        perror("fork failed");
        exit(1);
    }

    if (pid > 0) 
    {
        sender_message_queue("data/data.dat", bufferCapacity);
        int status = 0;
        waitpid(pid, &status, 0);
    } 
    else 
    {
        receiver_message_queue("data/data.ans");
    }

    return 0;
}

