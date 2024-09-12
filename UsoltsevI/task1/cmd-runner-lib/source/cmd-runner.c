#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include "../cmd-runner.h"
#include "../include/cmd-parser.h"

static void exec_cmd(char ***cmd) {
    pid_t pid;
    int fds[2];
    int fd_in = 0;

    int i = 0;
    while (cmd[i] != NULL) {
        if (pipe(fds)) {
            perror("pipe failed");
            return;
        }
        printf("New file descriptors: %d %d\n", fds[0], fds[1]);
        printf("input fds %d\n", fd_in);

        if ((pid = fork()) == -1) {
            perror("process creating");
            return;

        } else if (pid == 0) {
            if (i > 0) {
                dup2(fd_in, 0);
            }

            if (cmd[i + 1] != NULL) {
                dup2(fds[1],1);
            }

            close(fds[0]);

            execvp(cmd[i][0], cmd[i]);
            exit(1);

        } else {
            wait(NULL);
            if (i > 0) {
                close(fd_in);
            }
            close(fds[1]);
            fd_in = fds[0];
            ++i;
        }  
    }
    
    return;
}

void run_cmds(const char *cmd) {
    char ***cmds = parse_cmds(cmd);
    exec_cmd(cmds);
}
