#include "fdp_pipe_actions.h"
#include "fdp_pipe.h"

#include <unistd.h> 
#include <sys/types.h>

size_t parent_recieve(Pipe* self){  //direct_interact

    close(self->fd_back[1]);

    int red = read(self->fd_back[0], self->data, self->len);  //file len

    if(red < 0){
        fprintf(stderr, "File reading error\n");
        return -1;
    }

    int wrote = write(self->fd_output, self->data, self->len);

    fprintf(stderr, "Parent recieved: %d\n", wrote);

    return red;
}

size_t child_recieve(Pipe* self){  //direct_interact

    close(self->fd_direct[1]);

    int red = read(self->fd_direct[0], self->data, self->len);  //file len

    if(red < 0){
        printf("File reading error\n");
        return -1;
    }

    //self->len = red;

    return red;
}

size_t parent_send(Pipe* self){  //back interact

    close(self->fd_direct[0]);

    size_t red = read(self->fd_input, self->data, self->file_size);

    self->len = red;

    int wrote = write(self->fd_direct[1], self->data, self->len);

    if (wrote < 0){
        printf("File writing error\n");
        return -1;
    }

    return wrote;

}

size_t child_send(Pipe* self){  //back interact

    close(self->fd_back[0]);

    int wrote = write(self->fd_back[1], self->data, self->len);

    if (wrote < 0){
        printf("File writing error\n");
        return -1;
    }

    return wrote;

}

size_t get_file_size(const char* filename){
    size_t file_size = 0;

    FILE* fp = fopen(filename, "r");

    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    return file_size;
}


