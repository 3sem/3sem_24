#include "fdp_pipe_actions.h"
#include "fdp_pipe.h"

#include <unistd.h> 
#include <sys/types.h>

size_t parent_recieve(Pipe* self){  //direct_interact

    fprintf(stderr, "parent recieve\n");

    close(self->fd_back[1]);

    int red = read(self->fd_back[0], self->data, PIPE_BUF_SZ);  //file len

    if(red < 0){
        fprintf(stderr, "File reading error\n");
        return -1;
    }

    self->len = red;

    int wrote = write(self->fd_output, self->data, red);

    return red;
}

size_t child_recieve(Pipe* self){  //direct_interact

    fprintf(stderr, "child recieve\n");

    close(self->fd_direct[1]);

    int red = read(self->fd_direct[0], self->data, PIPE_BUF_SZ);  //file len

    if(red < 0){
        printf("File reading error\n");
        return -1;
    }

    self->len = red;

    return red;
}

size_t parent_send(Pipe* self){

    fprintf(stderr, "parent send\n");

    close(self->fd_direct[0]);

    size_t red = read(self->fd_input, self->data, PIPE_BUF_SZ);

    self->len = red;

    int wrote = write(self->fd_direct[1], self->data, PIPE_BUF_SZ);

    if (wrote < 0){
        printf("File writing error\n");
        return -1;
    }

    return wrote;

}

size_t child_send(Pipe* self){

    fprintf(stderr, "child send\n");

    close(self->fd_back[0]);

    int wrote = write(self->fd_back[1], self->data, self->len);

    if (wrote < 0){
        printf("File writing error\n");
        return -1;
    }

    return wrote;

}


