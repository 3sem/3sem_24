#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>

#define FIFO_NAME "/tmp/myfifo"  // FIFO file path

void sender_fifo(const char *input_file, size_t buffer_size) {
    // Create the FIFO (named pipe)
    if (mkfifo(FIFO_NAME, 0666) == -1) {
        perror("mkfifo failed");
        exit(1);
    }

    // Open FIFO for writing
    int fd = open(FIFO_NAME, O_WRONLY);
    if (fd == -1) {
        perror("open failed");
        exit(1);
    }

    // Read data from file and write to FIFO
    FILE *file = fopen(input_file, "rb");
    if (!file) {
        perror("Failed to open input file");
        exit(1);
    }
    char buffer[1024];
    size_t bytes_read = fread(buffer, 1, buffer_size, file);
    fclose(file);

    write(fd, buffer, bytes_read);
    printf("Parent: Data written to FIFO\n");

    close(fd);
}

void receiver_fifo(size_t buffer_size) {
    // Open FIFO for reading
    int fd = open(FIFO_NAME, O_RDONLY);
    if (fd == -1) {
        perror("open failed");
        exit(1);
    }

    // Read data from FIFO
    char buffer[1024];
    ssize_t bytes_read = read(fd, buffer, buffer_size);
    printf("Child: Data received from FIFO: %s\n", buffer);

    close(fd);
}

int main() {
    size_t buffer_size = 1024;  // Size of FIFO buffer (1 KB)
    const char *input_file = "input.dat";  // Replace with your file path

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork failed");
        exit(1);
    }

    if (pid > 0) {
        // Parent process (sender)
        sender_fifo(input_file, buffer_size);
        wait(NULL);  // Wait for child process to finish
    } else {
        // Child process (receiver)
        receiver_fifo(buffer_size);
        exit(0);
    }

    return 0;
}

