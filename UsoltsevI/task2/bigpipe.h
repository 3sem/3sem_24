#pragma once
#include <stdio.h>
#include <sys/types.h>

typedef struct pPipe Pipe;

Pipe  *construct_pipe(size_t size);
void   delete_pipe (Pipe *self);
void   pipe_set_pid(Pipe *self, int pid);
void   pipe_set_data_size(Pipe *self, size_t size);
size_t pipe_send_file   (Pipe *self, int fd);
size_t pipe_recieve_file(Pipe *self, int fd);
