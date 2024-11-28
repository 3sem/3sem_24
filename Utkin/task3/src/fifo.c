#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <assert.h>
#include <malloc.h>

#include "fifo.h"
#include "argvs.h"

int Init_Fifo() {return mkfifo(FIFONAME, 0666);}

int FIFO(int fread, int fwrite) {
    pid_t pid = fork();
    char buf[BUF_SIZE]; size_t len;
    if (pid < 0) perror("fork failed");
    else if (pid == 0) {
        int fd = open(FIFONAME, O_RDONLY);

        while (len = read(fd, buf, BUF_SIZE))
            write(fwrite, buf, len);
        close(fd);
        exit(EXIT_SUCCESS);
    } else {
        int fd = open(FIFONAME, O_WRONLY);
 
        while (len = read(fread, buf, BUF_SIZE))
            write(fd, buf, len);
        close(fd);
        unlink(FIFONAME);
    }

    int status = EXIT_FAILURE;
    wait(&status);
    if (status != EXIT_SUCCESS) perror("Error wait failed");

    return EXIT_SUCCESS;
}

int FIFOSendFile(int fread, int fwrite) {
    Init_Fifo();
    int res = FIFO(fread, fwrite);
    return res;
}