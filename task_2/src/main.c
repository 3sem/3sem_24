#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "pipe.h"
#include "IO_methods.h"

int main() {
    Pipe full_pipe = {};

    if(duplex_pipe_ctor(&full_pipe)) printf("Pipe ctor error\n");

    pid_t pid = fork();

    switch (pid)
    {
    case 0:
        receive_data_from_parent(&full_pipe);
        send_data_to_parent(&full_pipe);

        duplex_pipe_dtor(&full_pipe);
        exit(0);
        break;
    
    default:
        send_data_to_child(&full_pipe);
        receive_data_from_child(&full_pipe);

        duplex_pipe_dtor(&full_pipe);

        int status;
        waitpid(pid, &status, 0);
        break;
    }

    return 0;
}