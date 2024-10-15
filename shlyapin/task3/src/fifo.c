#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <assert.h>
#include <malloc.h>

#include "config.h"
#include "fifo.h"

int exchangeFifo(int fd_read, int fd_write) {
    if (mkfifo(FIFO_NAME, 0666) != 0) {
        perror("Error mkfifo failed");
        return EXIT_FAILURE; 
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        int fd_fifo_rd = open(FIFO_NAME, O_RDONLY);
        if (fd_fifo_rd == -1) {
            perror("Error open failed");
            exit(EXIT_FAILURE);
        }
        
        char *buf = malloc(BUF_SIZE);

        for (ssize_t len_msg = -1; len_msg != 0; ) {
            len_msg = read(fd_fifo_rd, buf, BUF_SIZE);
            if (len_msg < 0) {
                perror("Error read failed");
                exit(EXIT_FAILURE);                    
            }

            for (ssize_t len_wr = 0; len_wr < len_msg; ) {
                ssize_t cur_wr = write(fd_write, buf + len_wr, len_msg - len_wr);
                if (cur_wr < 0) {
                    perror("Error child write failed");
                    exit(EXIT_FAILURE);
                }
                len_wr += cur_wr;
            }
        }
        close(fd_fifo_rd);
        exit(EXIT_SUCCESS);
    } else {
        int fd_fifo_wr = open(FIFO_NAME, O_WRONLY);
        if (fd_fifo_wr == -1) {
            perror("Error open failed");
            exit(EXIT_FAILURE);
        }

        char *buf = malloc(BUF_SIZE);

        for (int len_msg = -1; len_msg != 0; ) {
            len_msg = read(fd_read, buf, BUF_SIZE);
            if (len_msg < 0) {
                perror("Error read failed");
                exit(EXIT_FAILURE);                    
            }

            for (ssize_t len_wr = 0; len_wr < len_msg; ) {
                ssize_t cur_wr = write(fd_fifo_wr, buf + len_wr, len_msg - len_wr);
                if (cur_wr < 0) {
                    perror("Error parent write failed");
                    exit(EXIT_FAILURE);
                }
                len_wr += cur_wr;
            }
        }
        close(fd_fifo_wr);
    }

    int status = EXIT_FAILURE;
    wait(&status);
    if (status != EXIT_SUCCESS) {
        perror("wait failed");
        exit(EXIT_FAILURE);
    }

    unlink(FIFO_NAME);

    return EXIT_SUCCESS;
}