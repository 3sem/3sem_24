#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <sys/wait.h>

#define FIFO_NAME "my_fifo"
#define BUFFER_SIZE 32*1024

void handle_error(const char *msg){
    perror(msg);
    exit(EXIT_FAILURE);
}

double get_time_diff(struct timespec start, struct timespec end){
    return (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
}

int main(int argc, char *argv[]){
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input_file> <output_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *input_file = argv[1];
    const char *output_file = argv[2];

    if (mkfifo(FIFO_NAME, 0666) == -1 && errno != EEXIST){
        handle_error("mkfifo");
    }

    pid_t pid = fork();
    if (pid == -1) {
        handle_error("fork");
    }

    if (pid == 0){
        // Дочерний процесс
        int fifo_fd = open(FIFO_NAME, O_RDONLY);
        if (fifo_fd == -1) {
            handle_error("open (reader)");
        }

        FILE *output = fopen(output_file, "wb");
        if (!output){
            handle_error("fopen (output file)");
        }

        char buffer[BUFFER_SIZE];
        ssize_t bytes_read;
        while ((bytes_read = read(fifo_fd, buffer, BUFFER_SIZE)) > 0){
            fwrite(buffer, 1, bytes_read, output);
        }

        fclose(output);
        close(fifo_fd);
        exit(EXIT_SUCCESS);

    } else {
        // Родительский процесс

        // Замер времени начала
        struct timespec start_time, end_time;
        if (clock_gettime(CLOCK_MONOTONIC, &start_time) == -1) {
            handle_error("clock_gettime (start)");
        }

        int fifo_fd = open(FIFO_NAME, O_WRONLY);
        if (fifo_fd == -1) {
            handle_error("open (writer)");
        }

        FILE *input = fopen(input_file, "rb");
        if (!input) {
            handle_error("fopen (input file)");
        }

        char buffer[BUFFER_SIZE];
        size_t bytes_read;
        while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, input)) > 0) {
            if (write(fifo_fd, buffer, bytes_read) == -1) {
                handle_error("write");
            }
        }

        fclose(input);
        close(fifo_fd);

        wait(NULL);

        // Замер времени окончания
        if (clock_gettime(CLOCK_MONOTONIC, &end_time) == -1) {
            handle_error("clock_gettime (end)");
        }

        unlink(FIFO_NAME);

        double elapsed_time = get_time_diff(start_time, end_time);
        printf("Time taken to transfer file: %.6f seconds\n", elapsed_time);
    }

    return EXIT_SUCCESS;
}