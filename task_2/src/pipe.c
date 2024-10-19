#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>

#include "pipe.h"

int duplex_pipe_ctor(Pipe* self) {
    if (!self) return -1;

    self->buffer = buffer_ctor(DEFAULT_INTERMIDIATE_BUFFER_SIZE);

    if (pipe(self->fd_direct)) return -1;   // create direct pipe
    if (pipe(self->fd_back))   return -1;   // create bach pipe

    return 0;
}

int duplex_pipe_dtor(Pipe* self) {
    if (!self) return -1;

    buffer_dtor(&(self->buffer));

    close(self->fd_direct[0]);
    close(self->fd_direct[1]);
    close(self->fd_back[0]);
    close(self->fd_back[1]);

    return 0;
}

Buffer buffer_ctor(size_t capacity) {
    Buffer buffer = {};

    buffer.data = (char*) calloc(capacity + 1, sizeof(char));
    assert(buffer.data);

    buffer.size = 0;
    buffer.capacity = capacity;

    return buffer;
}

int buffer_dtor(Buffer* buffer) {
    if (!buffer) return -1;

    free(buffer->data);

    buffer->size = 0;
    buffer->capacity = 0;

    return 0;
}