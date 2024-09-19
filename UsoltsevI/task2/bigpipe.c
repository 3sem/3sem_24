#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "bigpipe.h"

#define CHECK_NOT_NULL_RET_NULL(a, to_free1, to_free2)  \
    if (a == NULL) {                            \
        perror("null pointer");                 \
        free(to_free1);                         \
        free(to_free2);                         \
        return NULL;                            \
    }

#define CHECK_NOT_NULL_ARG(a, to_return)        \
    if (a == NULL) {                            \
        fprintf(stderr,                         \
                    "NULL argument in func %s\n"\
                        , __func__);            \
        return to_return;                       \
    }

#define CHECK_NOT_NULL_ARG_VOID(a)              \
    if (a == NULL) {                            \
        fprintf(stderr,                         \
                    "NULL argument in func %s\n"\
                        , __func__);            \
        return;                       \
    }

#define CHECK_TRUE_PRINT_ERROR(a)       \
    if (a) {                            \
        fprintf(stderr, "%s\n", #a);    \
    }

#define SAFE_EXECUTE(func, arg, to_free1, to_free2) \
    if (func(arg) == -1) {                  \
        perror("execution failed");         \
        free(to_free1);                     \
        free(to_free2);                     \
        return NULL;                        \
    }

typedef struct op_table Ops;

typedef struct op_table  {
    size_t (*recieve)(Pipe *self); 
    size_t (*send)   (Pipe *self); 
} Ops;

struct pPipe {
    char* data; // intermediate buffer
    int fd_direct[2]; // array of r/w descriptors for "pipe()" call (for parent-->child direction)
    int fd_back[2]; // array of r/w descriptors for "pipe()" call (for child-->parent direction)
    int pid;
    size_t len; // data length in intermediate buffer
    size_t data_size; // size of data to translate
    Ops actions;
};  

static size_t pipe_send_message(Pipe *self) {
    CHECK_NOT_NULL_ARG(self, 0);

    if (self->pid > 0) { // parent
        return write(self->fd_direct[1], self->data, self->len);
    }

    return write(self->fd_back[1], self->data, self->len);
}

static size_t pipe_recieve_message(Pipe *self) {
    CHECK_NOT_NULL_ARG(self, 0);

    if (self->pid > 0) { // parent
        return read(self->fd_back[0], self->data, self->len);
    }

    return read(self->fd_direct[0], self->data, self->len);
}    

Pipe *construct_pipe(size_t n) {
    Pipe *new_pipe  = (Pipe *) calloc(1, sizeof(Pipe));

    CHECK_NOT_NULL_RET_NULL(new_pipe, NULL, NULL);

    new_pipe->data_size = 0;
    new_pipe->len  = n;
    new_pipe->data = (char *) calloc(n, sizeof(char));

    CHECK_NOT_NULL_RET_NULL(new_pipe->data, new_pipe, NULL);

    new_pipe->actions.recieve = pipe_recieve_message;
    new_pipe->actions.send    = pipe_send_message   ;

    CHECK_NOT_NULL_RET_NULL(new_pipe->actions.recieve, new_pipe->data, new_pipe);
    CHECK_NOT_NULL_RET_NULL(new_pipe->actions.send   , new_pipe->data, new_pipe);

    SAFE_EXECUTE(pipe, new_pipe->fd_back  , new_pipe->data, new_pipe);
    SAFE_EXECUTE(pipe, new_pipe->fd_direct, new_pipe->data, new_pipe);

    return new_pipe;
}

void pipe_set_pid(Pipe *self, int pid) {
    CHECK_NOT_NULL_ARG_VOID(self)
    self->pid = pid;

    if (pid > 0) {
        close(self->fd_direct[0]);
        close(self->fd_back[1]);

    } else {
        close(self->fd_direct[1]);
        close(self->fd_back[0]);
    }
}

void pipe_set_data_size(Pipe *self, size_t size) {
    CHECK_NOT_NULL_ARG_VOID(self)
    self->data_size = size;
}

size_t pipe_send_file(Pipe *self, int fd) {
    CHECK_NOT_NULL_ARG(self, 0);

    int char_read = 0;
    size_t bytes_sent = 0;

    while ((char_read = read(fd, self->data, self->len)) > 0) {
        int shift = self->actions.send(self);
        bytes_sent += shift;

        // printf("bytes_sent: %ld\n", bytes_sent);

        CHECK_TRUE_PRINT_ERROR(shift != char_read);
    }

    printf("SENDING IS FINISHED\n");

    return bytes_sent;
}

size_t pipe_recieve_file(Pipe *self, int fd) {
    CHECK_NOT_NULL_ARG(self, 0);

    int char_read = 0;
    int char_write = 0;
    size_t bytes_recieved = 0;

    while ((char_read = self->actions.recieve(self)) > 0) {
        char_write = write(fd, self->data, char_read);
        bytes_recieved += char_read;

        if (bytes_recieved >= self->data_size) {
            break;
        }
        
        // printf("bytes_recieved: %ld, char_read: %d\n", bytes_recieved, char_read);

        CHECK_TRUE_PRINT_ERROR(char_read != char_write)
    }

    printf("RECIEVING IS FINISHED\n");

    return bytes_recieved;
}

void delete_pipe(Pipe *del_pipe) {
    CHECK_NOT_NULL_ARG_VOID(del_pipe);

    close(del_pipe->fd_back[0]);
    close(del_pipe->fd_back[1]);
    close(del_pipe->fd_direct[0]);
    close(del_pipe->fd_direct[1]);
    
    free(del_pipe->data);
    free(del_pipe);
}

#undef CHECK_NOT_NULL_RET_NULL
#undef CHECK_NOT_NULL_ARG
#undef CHECK_NOT_NULL_ARG_VOID
#undef CHECK_TRUE_PRINT_ERROR
#undef SAFE_EXECUTE
