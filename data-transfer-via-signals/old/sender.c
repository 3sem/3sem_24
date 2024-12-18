#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#define CHUNK_SIZE 4  // Chunk size in bytes

void send_file(pid_t receiver_pid, const char* file_path) 
{
    int fd = open(file_path, O_RDONLY);

    if (fd < 0) 
    {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    unsigned char buffer[CHUNK_SIZE];
    ssize_t bytes_read;
    size_t total_bytes_sent = 0;

    printf("[Sender] Starting file transfer...\n");

    while ((bytes_read = read(fd, buffer, CHUNK_SIZE)) > 0) 
    {
        if (bytes_read < CHUNK_SIZE) 
        {
            memset(buffer + bytes_read, 0, CHUNK_SIZE - bytes_read);
        }

        int payload = 0;

        for (int i = 0; i < CHUNK_SIZE; i++) 
        {
            payload = (payload << 8) | buffer[i];
        }

        union sigval sv;
        sv.sival_int = payload;
        if (sigqueue(receiver_pid, SIGRTMIN, sv) == -1) 
        {
            perror("Error sending signal");
            close(fd);
            exit(EXIT_FAILURE);
        }

        printf("[Sender] Sent %ld bytes: %02x %02x %02x %02x\n",
               bytes_read, buffer[0], buffer[1], buffer[2], buffer[3]);

        total_bytes_sent += bytes_read;

        usleep(10);  // Small delay to avoid queue overflow
    }

    union sigval sv;
    sv.sival_int = 0;  
    sigqueue(receiver_pid, SIGRTMIN, sv);

    printf("[Sender] File transfer complete. Total bytes sent: %zu\n", total_bytes_sent);

    close(fd);
}

int main(int argc, char* argv[]) 
{
    if (argc < 3) 
    {
        fprintf(stderr, "Usage: %s <receiver_pid> <file_to_send>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    pid_t receiver_pid = atoi(argv[1]);
    const char *file_path = argv[2];

    struct timespec start_time, end_time;
    double elapsed_time;

    if (clock_gettime(CLOCK_MONOTONIC, &start_time) == -1) 
    {
        perror("clock_gettime (start)");
        exit(EXIT_FAILURE);
    }

    send_file(receiver_pid, file_path);

    if (clock_gettime(CLOCK_MONOTONIC, &end_time) == -1) 
    {
        perror("clock_gettime (end)");
        exit(EXIT_FAILURE);
    }

    elapsed_time = (end_time.tv_sec - start_time.tv_sec) * 1e6 + 
                   (end_time.tv_nsec - start_time.tv_nsec) / 1e3;

    printf("Time taken to send signal: %.2f microseconds\n", elapsed_time);

    return 0;
}

