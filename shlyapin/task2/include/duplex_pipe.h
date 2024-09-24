#pragma once
#include <stdio.h>

typedef struct pPipe    Pipe;
typedef struct op_table Ops;

struct op_table  {
    ssize_t (*rcv)(Pipe *self); 
    ssize_t (*snd)(Pipe *self); 
};

struct pPipe {
        char* data;       // intermediate buffer
        size_t len;       // data length in intermediate buffer
        int fd_direct[2]; // array of r/w descriptors for "pipe()" call (for parent-->child direction)
        int fd_back[2];   // array of r/w descriptors for "pipe()" call (for child-->parent direction)
        Ops actions;
};

Pipe *ctor_pipe();
int dtor_pipe(Pipe *pp);
