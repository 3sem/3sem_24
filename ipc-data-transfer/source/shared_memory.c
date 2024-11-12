#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>
#include <string.h>
#include <time.h>
#include "buffer.h"

#define SHMEM_ID 12345
#define BUFFER_CAPACITY 1024

// Semaphore names
const char *PARENT_SEM_NAME = "/parentSem";
const char *CHILD_SEM_NAME = "/childSem";

sem_t *parentSem = NULL;
sem_t *childSem = NULL;

// Function prototypes
sem_t* init_semaphore(const char *name, int initial_value);
void cleanup_semaphore(sem_t *sem, const char *name);
void cleanup_shared_memory(int shmemId, char *shmaddr);

int receiver_shared_memory(const char* outputFilename, size_t bufferCapacity);
int sender_shared_memory(const char* inputFilename, size_t bufferCapacity);

int main() 
{
    // Unlink semaphores in case they exist from previous runs
    sem_unlink(PARENT_SEM_NAME);
    sem_unlink(CHILD_SEM_NAME);

    // Initialize semaphores
    parentSem = init_semaphore(PARENT_SEM_NAME, 1);
    childSem = init_semaphore(CHILD_SEM_NAME, 0);

    // Fork process to create parent (writer) and child (receiver)
    pid_t pid = fork();
    if (pid == -1) 
    {
        perror("fork error");
        return EXIT_FAILURE;
    }

    // Parent process (Writer)
    if (pid) 
    {
        sender_shared_memory("data/8KB.dat", SMALL_BUFFER_SIZE);
        
        int status;
        waitpid(pid, &status, 0);
    }
    // Child process (Receiver)
    else 
    {
        receiver_shared_memory("8KB_shm.ans", SMALL_BUFFER_SIZE);
    }

    return EXIT_SUCCESS;
}

int sender_shared_memory(const char* inputFilename, size_t bufferCapacity)
{
    Buffer* buffer = CreateBuffer(bufferCapacity);

    int fileDesc = OpenFile(inputFilename);
    if (fileDesc == -1) return EXIT_FAILURE;

    // Create shared memory
    int shmemId = shmget(SHMEM_ID, bufferCapacity + sizeof(size_t), IPC_CREAT | 0666);
    if (shmemId == -1) 
    {
        perror("shmget error");
        return EXIT_FAILURE;
    }

    char *shmaddr = shmat(shmemId, NULL, 0);
    if (shmaddr == (void *) -1) 
    {
        perror("shmat error");
        return EXIT_FAILURE;
    }

    size_t msgSize = 0;
    while ((msgSize = readFromFile(buffer, fileDesc)) > 0) 
    {
        sem_wait(parentSem); // Wait for receiver to be ready

        // Write to shared memory: first size, then buffer
        memcpy(shmaddr, &msgSize, sizeof(size_t));
        memcpy(shmaddr + sizeof(size_t), buffer->buffer, msgSize);

        sem_post(childSem); // Notify receiver
    }

    // Signal the receiver that transmission is complete
    sem_wait(parentSem);
    *((size_t *)shmaddr) = 0; // Zero size indicates end
    sem_post(childSem);

    // Clean up resources
    close(fileDesc);
    DestroyBuffer(buffer);
    cleanup_shared_memory(shmemId, shmaddr);
    cleanup_semaphore(parentSem, PARENT_SEM_NAME);

    // Wait for child process to finish
    printf("Parent: Data sent via shared memory\n");

    return 0;
}

int receiver_shared_memory(const char* outputFilename, size_t bufferCapacity)
{
    int shmemId = shmget(SHMEM_ID, bufferCapacity + sizeof(size_t), 0666);

    if (shmemId == -1) {
        perror("Child shmget error");
        return EXIT_FAILURE;
    }

    char *shmaddr = shmat(shmemId, NULL, 0);
    if (shmaddr == (void *)-1) {
        perror("Child shmat error");
        return EXIT_FAILURE;
    }

    int fileDesc = CreateFile(outputFilename);
    if (fileDesc == -1) return EXIT_FAILURE;

    while (1) {
        sem_wait(childSem); // Wait for data from sender

        size_t msgSize = *((size_t *)shmaddr);
        if (msgSize == 0) break; // Exit if size is zero (end of transmission)

        // Write data from shared memory to output file
        if (write(fileDesc, shmaddr + sizeof(size_t), msgSize) < 0) {
            perror("write error");
            return EXIT_FAILURE;
        }
        sem_post(parentSem); // Notify sender
    }

    // Clean up resources
    close(fileDesc);
    cleanup_shared_memory(shmemId, shmaddr);
    cleanup_semaphore(childSem, CHILD_SEM_NAME);

    printf("Child: Data received via shared memory\n");

    return 0;
}

// Function to initialize a semaphore
sem_t* init_semaphore(const char *name, int initial_value) {
    sem_t *sem = sem_open(name, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, initial_value);

    if (sem == SEM_FAILED) {
        perror("sem_open error");
        exit(EXIT_FAILURE);
    }

    return sem;
}

// Function to clean up a semaphore
void cleanup_semaphore(sem_t *sem, const char *name)
{
    if (sem != SEM_FAILED) 
    {
        sem_close(sem);
        sem_unlink(name);
    }
}

// Function to clean up shared memory
void cleanup_shared_memory(int shmemId, char *shmaddr) 
{
    shmdt(shmaddr);
    shmctl(shmemId, IPC_RMID, NULL);
}

