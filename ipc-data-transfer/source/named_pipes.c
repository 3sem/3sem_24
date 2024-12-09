#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <string.h>
#include "buffer.h"

#define FIFO_NAME "my_fifo"  // FIFO file path

int sender_fifo(const char* inputFilename, size_t bufferCapacity) 
{
    // Create the FIFO (named pipe)
    if (mkfifo(FIFO_NAME, 0666) == -1) {
        perror("mkfifo failed");
        exit(1);
    }

    Buffer* buffer = CreateBuffer(bufferCapacity);

    int outFileDesc = open(FIFO_NAME, O_WRONLY);

    if (outFileDesc == -1)
    {
        perror("Unable to open fifo");

        return EXIT_FAILURE;
    }

    int inFileDesc = OpenFile(inputFilename);

    if (inFileDesc == -1)
    {
        perror("Unable to open input file");

        return EXIT_FAILURE;
    }

    while (readFromFile(buffer, inFileDesc))
    {
        writeToFile(buffer, outFileDesc);
    }

    printf("Parent: Data sent via fifo\n");

    DestroyBuffer(buffer);
    close(outFileDesc);
    close(inFileDesc);

    return 0;
}

int receiver_fifo(const char* outputFilename, size_t bufferCapacity) 
{
    Buffer* buffer = CreateBuffer(bufferCapacity);

    int inFileDesc = open(FIFO_NAME, O_RDONLY);
    if (inFileDesc == -1) 
    {
        perror("Unable to open file");

        return EXIT_FAILURE;
    }

    int outFileDesc = CreateFile(outputFilename);

    if (outFileDesc == -1)
    {
        perror("Unable to create file");

        return EXIT_FAILURE;
    }

    ssize_t bytesRead;
    while ((bytesRead = read(inFileDesc, buffer->buffer, bufferCapacity)) > 0) {
        if (write(outFileDesc, buffer->buffer, bytesRead) == -1) {
            perror("write to output file failed");
            break;
        }
    }

    printf("Child: Data received via fifo\n");

    DestroyBuffer(buffer);
    close(inFileDesc);
    close(outFileDesc);


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

    unlink(FIFO_NAME);

    pid_t pid = fork();

    if (pid == -1) 
    {
        perror("fork failed");
        exit(1);
    }

    if (pid > 0) 
    {
        sender_fifo("data/data.dat", bufferCapacity);
        int status = 0;
        waitpid(pid, &status, 0);
    } 
    else 
    {
        receiver_fifo("data/data.ans", bufferCapacity);
    }

    return 0;
}

