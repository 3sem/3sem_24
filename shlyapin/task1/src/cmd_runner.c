#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "cmd_runner.h"
#include "parser.h"
#include "logger.h"

int run_cmd(const char *cmd_ptr) {
    char **cmds = parse_cmd(cmd_ptr, "|");
    
    int num_cmds = 0;
    while (cmds[num_cmds] != NULL) {
        num_cmds++;
    }

    int pipe_fd[2];
    int prev_fd = -1;

    for (int i = 0; i < num_cmds; i++) {
        char **args = parse_cmd(cmds[i], " ");

        if (i < num_cmds - 1) {
            if (pipe(pipe_fd) == -1) {
                PRINT_ERROR("Pipe failed\n");
                return -1;
            }
        }

        pid_t pid = fork();
        if (pid < 0) {
            PRINT_ERROR("Fork failed\n");
            return -1;
        } else if (pid == 0) {
            if (prev_fd != -1) {
                dup2(prev_fd, STDIN_FILENO);
                close(prev_fd);
            }

            if (i < num_cmds - 1) {
                close(pipe_fd[0]);
                dup2(pipe_fd[1], STDOUT_FILENO);
                close(pipe_fd[1]);
            }

            execvp(args[0], args);
            PRINT_ERROR("Exec failed\n");
            exit(EXIT_FAILURE);
        }

        if (prev_fd != -1) {
            close(prev_fd);
        }

        if (i < num_cmds - 1) {
            close(pipe_fd[1]);
            prev_fd = pipe_fd[0];
        }

        free_parsed_cmd(args);
    }

    for (int i = 0; i < num_cmds; i++) {
        int status;
        wait(&status);
    }

    free_parsed_cmd(cmds);

    return 0;
}
