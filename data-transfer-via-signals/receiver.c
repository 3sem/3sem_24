#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <semaphore.h>

#define CHUNK_SIZE 4 

sem_t* semaphore;

int done = 0; 
int output_fd;
size_t total_bytes_received = 0;

void signal_handler(int signo, siginfo_t* info, void* context) 
{
    if (info->si_value.sival_int == 0) 
    {
        done = 1;  // End of file
        printf("[Receiver] End of file signal received.\n");
        return;
    }

    // Decode the payload into bytes
    int payload = info->si_value.sival_int;
    unsigned char chunk[CHUNK_SIZE];
    for (int i = CHUNK_SIZE - 1; i >= 0; i--) 
    {
        chunk[i] = payload & 0xFF;
        payload >>= 8;
    }

    // Write the chunk directly to the output file
    if (write(output_fd, chunk, CHUNK_SIZE) < 0) 
    {
        perror("Error writing to output file");
        exit(EXIT_FAILURE);
    }

    total_bytes_received += CHUNK_SIZE;

    printf("[Receiver] Received %d bytes: %02x %02x %02x %02x\n",
           CHUNK_SIZE, chunk[0], chunk[1], chunk[2], chunk[3]);

    sem_post(semaphore);
}

int main(int argc, char *argv[]) 
{
    if (argc < 2) 
    {
        fprintf(stderr, "Usage: %s <output_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    semaphore = sem_open("/signal_semaphore", O_CREAT, 0644, 1);
    if (semaphore == SEM_FAILED) 
    {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    const char* output_file = argv[1];

    // Open the output file
    output_fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (output_fd < 0) 
    {
        perror("Error opening output file");
        exit(EXIT_FAILURE);
    }

    // Set up the signal handler
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = signal_handler;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGRTMIN, &sa, NULL) == -1) 
    {
        perror("Error setting up signal handler");
        close(output_fd);
        exit(EXIT_FAILURE);
    }

    printf("[Receiver] Receiver PID: %d\n", getpid());
    printf("[Receiver] Waiting for file...\n");

    // Wait for signals until the file transfer is complete
    while (!done) 
    {
        pause();
    }

    printf("[Receiver] File transfer complete. Total bytes received: %zu\n", total_bytes_received);

    // Close the output file
    close(output_fd);

    sem_close(semaphore);
    return 0;
}

