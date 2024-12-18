#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>
#include <time.h>

#define BUFFER_SIZE 64*1024

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Use: %s <input> <output>\n", argv[0]);
        return EXIT_FAILURE;
    }

    struct timespec start, stop;

    mkfifo("fifofifo", 0666);

    pid_t pid = fork();

    char buffer[BUFFER_SIZE];
    
    clock_gettime(CLOCK_MONOTONIC, &start);

    if (pid == 0) {
        
        int fd = open("fifofifo", O_RDONLY);
        int out = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0666);

        ssize_t bytes_read;
        while ((bytes_read = read(fd, buffer, BUFFER_SIZE)) > 0) {
            if (write(out, buffer, bytes_read) == -1) {
                perror("Write error");
                break;
            }
        }

        printf("Чтение FIFO завершено\n");
        close(fd);
        close(out);
        exit(EXIT_SUCCESS);
    }

    else if (pid > 0) {
        int input = open(argv[1], O_RDONLY);

        int fd = open("fifofifo", O_WRONLY);

        ssize_t bytes_read;
        while ((bytes_read = read(input, buffer, BUFFER_SIZE)) > 0) {
            if (write(fd, buffer, bytes_read) == -1) {
                perror("Fifo write error");
                break;
            }
        }

        printf("Запись FIFO завершена\n");
        close(fd);
        close(input);

        wait(NULL);

        unlink("fifofifo");
    }

    clock_gettime(CLOCK_MONOTONIC, &stop);
    double exec_time = (stop.tv_sec - start.tv_sec) + (stop.tv_nsec - start.tv_nsec) / (double) 1e9;
    printf("Время работы: %.4f с\n", exec_time);   

    return 0;
}