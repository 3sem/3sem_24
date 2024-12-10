#pragma once 
#include <stdio.h>

#define PIPE_BUF_SZ 4096

typedef struct pPipe Pipe;

typedef struct op_table Ops;
typedef struct op_table  {
    size_t (*rcv)(Pipe *self); 
    size_t (*snd)(Pipe *self); 
} Ops;

typedef struct pPipe {
        char data[PIPE_BUF_SZ]; // intermediate buffer
        int fd_direct[2]; // array of r/w descriptors for "pipe()" call (for parent-->child direction)
        int fd_back[2]; // array of r/w descriptors for "pipe()" call (for child-->parent direction)
        int fd_output;
        int fd_input; 
        const char* fn;
        size_t file_size;
        size_t len; // data length in intermediate buffer
        Ops actions;
} Pipe;

Pipe* pipe_ctor(void);
void pipe_dtor(Pipe* pipe);