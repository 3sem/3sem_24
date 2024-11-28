#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "pipe.h"

Pipe* c_pipe2() {
    Pipe* pipe2 = calloc(1, sizeof(Pipe));
    pipe(pipe2->fd_back);
    pipe(pipe2->fd_direct);
    pipe2->actions.rcv = recieve;
    pipe2->actions.snd = send;
    return pipe2;
}

size_t send(Pipe* pipe2) {
    size_t c = 0;
    if (fcntl(pipe2->fd_back[1], F_GETFL) == -1)
        c = write(pipe2->fd_direct[1], pipe2->data, pipe2->len);
    else
        c = write(pipe2->fd_back[1], pipe2->data, pipe2->len);
    return c;
}

size_t recieve(Pipe* pipe2) {
    size_t c = 0;
    if (fcntl(pipe2->fd_back[0], F_GETFL) == -1)
        c = read(pipe2->fd_direct[0], pipe2->data, pipe2->len);
    else
        c = read(pipe2->fd_back[0], pipe2->data, pipe2->len);
    return c;
}