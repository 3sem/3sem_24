#pragma once
#include <stdio.h>
#include <sys/types.h>

typedef struct pPipe Pipe;

Pipe *construct_pipe(size_t size);
size_t pipe_send_file(Pipe *self, pid_t pid, int fd);
size_t pipe_recieve_file(Pipe *self, pid_t pid, int fd);
void delete_pipe(Pipe *self);
