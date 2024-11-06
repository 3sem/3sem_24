#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#include "fdp_pipe.h"
#include "fdp_pipe_actions.h"


//file.txt -> родитель читатет -> ребенок принимает -> обрабатывает -> отправляет обратно родителю -> родитель пишет в output.txt
//parnet send -> child recieve -> child send -> parent recieve

int main(int argc, const char* argv[]){

    Pipe* dpipe = pipe_ctor();
    
    /*switch(argc){                                   //выбираем имя input файла
        case 0:{
            const char* filename = "file.txt";
            truba->fd_input = open(filename, O_RDWR);
        }

        case 1:{
            const char* filename = argv[1];
            truba->fd_input = open(filename, O_RDWR);
        }

        default:{
            printf("Argc error\n");
            return -1;
        }
    }*/

    const char* filename = "input.txt";
    dpipe->fd_input = open(filename, O_RDWR);
    
    dpipe->fd_output = open("output.txt", O_RDWR);

    pid_t pid = fork();

    switch(pid){
        case -1:{
            printf("child creation error\n");
            exit(-1);
        }

        case 0:{                    
            //CHILD CASE
            dpipe->actions.rcv(dpipe);

            dpipe->actions.snd = child_send;

            dpipe->actions.snd(dpipe);

            sleep(5);

            break;
        }

        default:{
            //PARENT CASE

            fprintf(stderr, "switch parent send\n");

            dpipe->actions.snd(dpipe);

            fprintf(stderr, "switch parent recieve\n");

            dpipe->actions.rcv = parent_recieve;

            dpipe->actions.rcv(dpipe);

            break;
        }
    }

    fprintf(stderr, "end of prog\n");
    
    pipe_dtor(dpipe);

    return 0;
}