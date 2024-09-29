//-----------------------------------------------------------------
//
// FIFO file translation implementation
//
//-----------------------------------------------------------------

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "../include/fifo.h"
#include "../include/define-check-condition-ret.h"

static const size_t FIFO_BUF_SIZE = 1024;

int fifo_translate_file(int fd, size_t file_size) {
    int pipefd[2];
    int res = pipe(pipefd);
    CHECK_CONDITION_PERROR_RET(res != 0, "pipe", 1);

    pid_t pid = fork();
    CHECK_CONDITION_PERROR_RET(pid < 0, "fork", 1);
    
    if (pid > 0) {
        close(pipefd[1]);

        char *buf = (char *) calloc(FIFO_BUF_SIZE, sizeof(char));
        CHECK_CONDITION_PERROR_RET(buf == NULL, "calloc", 1);

        size_t char_read = 0;
        size_t char_rcvd = 0;

        while ((char_read = read(pipefd[0], buf, FIFO_BUF_SIZE)) > 0) {
            char_rcvd += char_read;
            if (char_rcvd >= file_size) {
                break;
            }
        }
        
        free(buf);

        if (char_rcvd >= file_size) {
            return 0;
        }

    } else {
        close(pipefd[0]);
        
        char *buf = (char *) calloc(FIFO_BUF_SIZE, sizeof(char));
        CHECK_CONDITION_PERROR_RET(buf == NULL, "calloc", 1);

        size_t char_read = 0;
        size_t char_sent = 0;
        size_t char_wrtn = 0;

        while ((char_read = read(fd, buf, FIFO_BUF_SIZE)) > 0) {
            char_wrtn = write(pipefd[1], buf, char_read);
            CHECK_CONDITION_PRINT(char_wrtn != char_read)
            char_sent += char_wrtn;
            if (char_sent >= file_size) {
                break;
            }
        }

        free(buf);
        exit(0);
    }
    
    return 1;
}