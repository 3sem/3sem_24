#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include "separator.h"
#include "processor.h"

int process_child(char* cmd, char** args, int* pipe0, int pipe_fd[2], int num_cmds, int current_cmd) {
    char* delim = " ";
    int num_args = separator(cmd, args, delim);

    if (*pipe0 != -1) {
        dup2(*pipe0, STDIN_FILENO);
        close(*pipe0);
    }

    if (current_cmd < num_cmds - 1) {
        close(pipe_fd[0]);
        dup2(pipe_fd[1], STDOUT_FILENO);
        close(pipe_fd[1]);
    }

    return execvp(args[0], args);
    exit(0);
}

int processed(char* mem) {
    char** cmds = calloc(MAX_LENGHT, sizeof(char*));
    char** args = calloc(MAX_LENGHT, sizeof(char*));
    char* delim0 = "|"; char* delim = " ";

    int num_cmds = separator(mem, cmds, delim0);

    pid_t pids[num_cmds];
    int pipe_fd[2]; int pipe0 = -1; 

    for (int i = 0; i < num_cmds; i++) {
        if (i < num_cmds - 1) pipe(pipe_fd);
        pids[i] = fork();

        if (pids[i] < 0) printf("Pipe Error\n");
        else if (pids[i] == 0) {
            //process_child(cmds[i], args, &pipe0, pipe_fd, num_cmds, i);
            char* delim = " ";
            int num_args = separator(cmds[i], args, delim);

            if (pipe0 != -1) {
                dup2(pipe0, STDIN_FILENO);
                close(pipe0);
            }

            if (i < num_cmds - 1) {
                close(pipe_fd[0]);
                dup2(pipe_fd[1], STDOUT_FILENO);
                close(pipe_fd[1]);
            }

            execvp(args[0], args);
            exit(EXIT_FAILURE);
        }

        if (pipe0 != -1) close(pipe0);

        if (i < num_cmds - 1) {
            close(pipe_fd[1]);
            pipe0 = pipe_fd[0];
        }
    }
    int status = 100;

    for (int i = 0; i < num_cmds; i++) wait(&status);
    printf("[Exit status: %d]\n", status);

    free(cmds); free(args);

    return status;
}