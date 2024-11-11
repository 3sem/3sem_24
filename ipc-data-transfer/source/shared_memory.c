#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <unistd.h>

#define SHM_KEY 1234  // Shared memory key

void sender_shared_memory(const char *input_file, size_t buffer_size) {
    // Create shared memory segment
    int shm_id = shmget(SHM_KEY, buffer_size, IPC_CREAT | 0666);
    if (shm_id == -1) {
        perror("shmget failed");
        exit(1);
    }

    char *shm_ptr = (char *)shmat(shm_id, NULL, 0);
    if (shm_ptr == (char *)-1) {
        perror("shmat failed");
        exit(1);
    }

    // Write data to shared memory (simulating reading a file)
    FILE *file = fopen(input_file, "rb");
    if (!file) {
        perror("Failed to open input file");
        exit(1);
    }
    fread(shm_ptr, 1, buffer_size, file);
    fclose(file);

    printf("Parent: Data written to shared memory\n");

    // Detach shared memory
    shmdt(shm_ptr);
}

void receiver_shared_memory(size_t buffer_size) {
    // Attach to the shared memory segment
    int shm_id = shmget(SHM_KEY, buffer_size, 0666);
    if (shm_id == -1) {
        perror("shmget failed");
        exit(1);
    }

    char *shm_ptr = (char *)shmat(shm_id, NULL, 0);
    if (shm_ptr == (char *)-1) {
        perror("shmat failed");
        exit(1);
    }

    // Read data from shared memory
    printf("Child: Data read from shared memory: %s\n", shm_ptr);

    // Detach shared memory
    shmdt(shm_ptr);
}

int main() {
    size_t buffer_size = 1024; // Size of shared memory (1 KB for simplicity)
    const char *input_file = "input.dat";  // Replace with your file path

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork failed");
        exit(1);
    }

    if (pid > 0) {
        // Parent process (sender)
        sender_shared_memory(input_file, buffer_size);
        wait(NULL);  // Wait for child process to finish
    } else {
        // Child process (receiver)
        receiver_shared_memory(buffer_size);
        exit(0);
    }

    return 0;
}

