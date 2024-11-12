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

int main() 
{
    unlink(FIFO_NAME);

    pid_t pid = fork();

    if (pid == -1) 
    {
        perror("fork failed");
        exit(1);
    }

    if (pid > 0) 
    {
        sender_fifo("data/8KB.dat", SMALL_BUFFER_SIZE);
        int status = 0;
        waitpid(pid, &status, 0);
    } 
    else 
    {
        receiver_fifo("8KB_fifo.ans", SMALL_BUFFER_SIZE);
    }

    return 0;
}

