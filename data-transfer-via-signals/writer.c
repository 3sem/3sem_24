#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/shm.h>
#include <sys/wait.h>
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
    if (signo == SIGUSR2) 
    {
        ready = 1;
        printf("[writer] Received acknowledgment from Process 2\n");
    }
}

int main(int argc, char* argv[]) 
{
    if (argc != 3) 
    {
        fprintf(stderr, "Usage: %s <input_file> <shm_key>\n", argv[0]);

        exit(1);
    }

    const char* input_file = argv[1];
    key_t key = atoi(argv[2]);

    // Create shared memory
    int shmid = shmget(key, sizeof(SharedMemory), IPC_CREAT | 0666);

    if (shmid < 0) 
    {
        perror("shmget");
        exit(1);
    }

    SharedMemory* shm = (SharedMemory *)shmat(shmid, NULL, 0);

    if (shm == (SharedMemory *) -1) 
    {
        perror("shmat");

        exit(1);
    }

    // Set up signal handling
    signal(SIGUSR2, sig_handler);

    printf("[writer] Started with PID: %d\n", getpid());

    // Open the input file
    FILE *fp = fopen(input_file, "rb");

    if (!fp) 
    {
        perror("fopen");

        exit(1);
    }

    // Fork to create reader process 
    pid_t pid = fork();

    if (pid < 0) 
    {
        perror("fork");

        exit(1);
    }

    if (pid == 0) 
    { // Child process
        execl("./reader", "reader", argv[2], NULL);
        perror("execl");
        exit(1);
    }

    printf("[writer] Forked reader process with PID: %d\n", pid);
    sleep(1); // Give Process 2 time to set up

    size_t bytes_read;
    while ((bytes_read = fread(shm->data, 1, SHM_SIZE, fp)) > 0) 
    {
        shm->size = bytes_read;
        shm->status = DATA_READY;

        printf("[writer] Read %zu bytes from file, sending to reader\n", bytes_read);

        // Notify Process 2 that data is ready
        kill(pid, SIGUSR1);
        printf("[writer] Sent SIGUSR1 to reader (PID: %d)\n", pid);

        // Wait for acknowledgment from Process 2
        while (!ready) pause();
        ready = 0;
    }

    // Indicate transfer completion
    shm->status = TRANSFER_COMPLETE;
    kill(pid, SIGUSR1);
    printf("[writer] Transfer complete, sent SIGUSR1 to reader (PID: %d)\n", pid);

    // Clean up
    fclose(fp);
    shmdt(shm);
    shmctl(shmid, IPC_RMID, NULL);

    // Wait for Process 2 to finish
    wait(NULL);
    printf("[writer] reader has finished. Exiting...\n");
    return 0;
}

