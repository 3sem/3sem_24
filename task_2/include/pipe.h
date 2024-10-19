#ifndef PIPE_H
#define PIPE_H

#define DEFAULT_INTERMIDIATE_BUFFER_SIZE 4096

typedef struct _Buffer {
    char*  data;
    size_t size;
    size_t capacity;
} Buffer;

typedef struct pPipe {
    Buffer buffer;
    int fd_direct[2];   // array of r/w descriptors for "pipe()" call (for parent-->child direction)
    int fd_back[2];     // array of r/w descriptors for "pipe()" call (for child-->parent direction)
} Pipe;

/* Data send scheme:
    File -> intermediate buffer(data) -> Pipe(fd_direct[1] or fd_back[1])
    Than wait for cleaning Pipe
*/

int duplex_pipe_ctor(Pipe* self);
int duplex_pipe_dtor(Pipe* self);

Buffer buffer_ctor(size_t capacity);
int buffer_dtor(Buffer* buffer);

#endif