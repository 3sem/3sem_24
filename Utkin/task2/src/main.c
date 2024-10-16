#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "pipe.h"

#define MAX_SIZE 4096

int sendfile(char* files[], Pipe* pipe2, char buf[]) {
    pid_t pid = fork();
    if (pid == 0) {
        close(pipe2->fd_direct[1]); close(pipe2->fd_back[0]);
        int file = open(files[1], O_RDONLY);
        size_t c = 0; size_t id = 0;
        while (c = read(file, buf, MAX_SIZE)) {
            pipe2->data = buf; pipe2->len = c;
            pipe2->actions.snd(pipe2);
        }
        close(pipe2->fd_back[1]);
        close(file);
    } else {
        close(pipe2->fd_direct[0]); close(pipe2->fd_back[1]);
        int file = open(files[2], O_WRONLY | O_CREAT, 0666);
        size_t c = MAX_SIZE;
        pipe2->len = MAX_SIZE; pipe2->data = buf;

        while (c = pipe2->actions.rcv(pipe2)) {
            write(file, buf, c);
        }

        close(file);
    }
}

int main(int argc, char* argv[]) {
    Pipe* pipe2 = c_pipe2();
    char buf[MAX_SIZE];

    sendfile(argv, pipe2, buf);
    
    free(pipe2);
    return 0;
}