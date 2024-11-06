#include <stdio.h>
#include <fcntl.h>
#include <unistd.h> 
#include <stdlib.h>

#include "fdp_pipe.h"
#include "fdp_pipe_actions.h"

Pipe* pipe_ctor(void){
    Pipe* dpipe = (Pipe*)calloc(1, sizeof(Pipe));
    dpipe->len = 0;

    dpipe->actions.rcv = child_recieve;
    dpipe->actions.snd = parent_send;

    if (pipe(dpipe->fd_direct) < 0){
        printf("Pipe creation error\n");
        return NULL;
    }
    if (pipe(dpipe->fd_back) < 0){
        printf("Pipe creation error\n");
        return NULL;
    }
    return dpipe;
}

void pipe_dtor(Pipe* dpipe){
    close(dpipe->fd_back[0]);
    close(dpipe->fd_back[1]);

    close(dpipe->fd_direct[0]);
    close(dpipe->fd_direct[1]);

    close(dpipe->fd_input);
    close(dpipe->fd_output);
    
    free(dpipe);
}