#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "pipe.h"
#include "IO_methods.h"
#include "IO_settings.h"

int main() {
    
    Pipe full_pipe = {};

    if(duplex_pipe_ctor(&full_pipe)) printf("Pipe ctor error\n");

    pid_t pid = fork();

    switch (pid)
    {
    case 0:
        {
            File file = create_file(CHILD_TO_PARENT);
            close(full_pipe.fd_back[0]);

            File receive_file = create_file(CHILD_RECEIVED_FILE);
            close(full_pipe.fd_direct[1]);   // close write descriptor 

            while(send_data_to_parent(&full_pipe, &file))
            {
                receive_data_from_parent(&full_pipe, &receive_file);
            }

            close(full_pipe.fd_back[1]);

            receive_data_from_parent(&full_pipe, &receive_file);

            delete_file(&file);
            delete_file(&receive_file);

            duplex_pipe_dtor(&full_pipe);
            exit(0);
            break;
        }
    
    default:
        {
            File file = create_file(PARENT_TO_CHILD);
            close(full_pipe.fd_direct[0]);

            File receive_file = create_file(PARENT_RECEIVED_FILE);
            close(full_pipe.fd_back[1]);   // close write descriptor 

            while(send_data_to_child(&full_pipe, &file)) {
                receive_data_from_child(&full_pipe, &receive_file);
            }

            close(full_pipe.fd_direct[1]);

            receive_data_from_child(&full_pipe, &receive_file);

            delete_file(&file);
            delete_file(&receive_file);

            duplex_pipe_dtor(&full_pipe);

            int status;
            waitpid(pid, &status, 0);
            break;
        }
    }

    return 0;
}