#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/shm.h>
#include <fcntl.h>

#define SHM_SIZE 4096
#define DATA_READY 1
#define TRANSFER_COMPLETE 2

typedef struct 
{
    int status;
    size_t size;
    char data[SHM_SIZE];
} SharedMemory;

volatile sig_atomic_t ready = 0;

void sig_handler(int signo) 
{
    if (signo == SIGUSR1) ready = 1;
}

int main(int argc, char *argv[]) 
{
    if (argc != 2) 
    {
        fprintf(stderr, "Usage: %s <shm_key>\n", argv[0]);
        exit(1);
    }

    key_t key = atoi(argv[1]);

    // Attach to the shared memory
    int shmid = shmget(key, sizeof(SharedMemory), 0666);
    if (shmid < 0) 
    {
        perror("shmget");
        exit(1);
    }

    SharedMemory* shm = (SharedMemory *)shmat(shmid, NULL, 0);

    if (shm == (SharedMemory *)-1) 
    {
        perror("shmat");
        exit(1);
    }

    // Setup signal handling
    signal(SIGUSR1, sig_handler);

    printf("[reader] Started with PID: %d\n", getpid());
    printf("[reader] Waiting for data...\n");

    FILE *fp = fopen("output.dat", "wb");

    if (!fp)
    {
        perror("fopen");
        exit(1);
    }

    // Main loop to read data from shared memory
    while (1) 
    {
        while (!ready) pause(); // Wait for signal from Process 1
        ready = 0;

        if (shm->status == DATA_READY) 
        {
            printf("[reader] Received data block of size: %zu bytes\n", shm->size);
            
            size_t bytes_written = fwrite(shm->data, 1, shm->size, fp);

            if (bytes_written != shm->size) 
            {
                fprintf(stderr, "[reader] Error writing to file\n");
            } 
            else 
            {
                printf("[reader] Successfully wrote %zu bytes to file\n", bytes_written);
            }
            
            fflush(fp);
            
            // Send acknowledgment to Process 1
            kill(getppid(), SIGUSR2);
            printf("[reader] Sent acknowledgment to Process 1 (PID: %d)\n", getppid());
        } 
        else if (shm->status == TRANSFER_COMPLETE) 
        {
            printf("[reader] Transfer complete. Exiting...\n");
            break;
        }
    }

    fclose(fp);
    shmdt(shm);
    return 0;
}

