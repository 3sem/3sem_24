#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/time.h>

#define BUF_SIZE 1024*1024

int main(int argc, char *argv[]) {
    struct timeval start, end;
    double time_spent;

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input_file> <output_file>\n", argv[0]);
        return 1;
    }

    if (unlink("myfifo") == -1 && errno != ENOENT) {
        perror("unlink");
        return 1;
    }

    if (mkfifo("myfifo", 0777) < 0) {
        perror("mkfifo");
        return 1;
    }

    gettimeofday(&start, NULL);
    pid_t child_pid = fork();
    char buffer[BUF_SIZE];

    if (child_pid == 0) { 
        int fd = open("myfifo", O_RDONLY);
        int output_file = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0666);
        
        if (output_file == -1) {
            perror("open output file");
            return 1;
        }
          
        if (fd == -1) {
            perror("open fifo");
            close(output_file);
            return 1;
        }

        ssize_t bytes_read;
        while ((bytes_read = read(fd, buffer, BUF_SIZE)) > 0) {
            if (write(output_file, buffer, bytes_read) == -1) {
                perror("error write in file");
                break;
            }
        }
        
        printf("Child process finished\n");
        close(fd);
        close(output_file);
        exit(EXIT_SUCCESS);
    } else if (child_pid > 0) {
        int input_file = open(argv[1], O_RDONLY);
        if (input_file == -1) {
            perror("Error opening input file");
            unlink("myfifo");
            exit(EXIT_FAILURE);
        }

        int fd = open("myfifo", O_WRONLY);
        if (fd == -1) {
            perror("Error opening fifo");
            close(input_file);
            unlink("myfifo");
            exit(EXIT_FAILURE);
        }
          
        ssize_t bytes_read;
        while ((bytes_read = read(input_file, buffer, BUF_SIZE)) > 0) {
            if (write(fd, buffer, bytes_read) == -1) {
                perror("error write in fifo");
                break;
            }
        }

        close(fd);
        close(input_file);
        printf("Parent process finished\n");
        
        wait(NULL);
        gettimeofday(&end, NULL);
        time_spent = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
        printf("Time spent: %.6f seconds\n", time_spent);
        
        unlink("myfifo");
    } else {
        perror("fork");
        unlink("myfifo");
        return 1;
    }

    return 0;
}
