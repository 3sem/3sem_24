#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

void run_cmd(char*** cmd){
    pid_t pid;
    int fds[2];
    int fd_in = STDIN_FILENO;

    int i = 0;
    while (cmd[i]!= NULL) {
        pipe(fds);
        //printf("New file descriptions: %d %d\n", fds[0], fds[1]);
        //printf("input fds %d\n", fd_in);
        
        if ((pid = fork()) == -1) {
            perror("process creating");
            return;
        }
        else if (pid == 0){
            if (i>0) {
                dup2(fd_in, STDIN_FILENO);
            }
            if (cmd[i+1] != NULL) {
                dup2(fds[1], STDOUT_FILENO);
            }
            close(fds[0]);
            execvp(cmd[i][0], cmd[i]);
            printf("Wrong command\n");
            exit(1);
        }
        else { 
            int status;
            waitpid(pid, &status, 0);
            //printf("exit code: %d\n", status);

            if (i>0) close(fd_in);
            close(fds[1]);
            fd_in = fds[0];
            
            i++;
        }
    }
    
}
