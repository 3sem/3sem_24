#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "incl/execcmd.h"
#include "incl/nums.h"
#include "incl/parse.h"

#define CMD_LEN 256

void exec_input_commands(char *cmd) {
	char *commands[CMD_LEN];
	int commands_num = num(cmd, commands);	


	int fd[2];
	int fd_in = 0;

	pid_t pid;

	for(int i = 0; i < commands_num; i++) {
		pipe(fd);

		char *args[CMD_LEN];

		pid = fork();

		if(pid < 0) {
			perror("Fork failed");
		}
		else if(pid == 0) {
			dup2(fd_in, 0);
			if(i < commands_num - 1) {
				dup2(fd[1], 1);
			}
			close(fd[0]);
			parse(commands[i], args);
			execvp(args[0], args);
			exit(EXIT_FAILURE);
		}
		else {
			int status;
			waitpid(pid, &status, 0);
			if(i != 0) {
				close(fd_in);
			}
			close(fd[1]);
			fd_in = fd[0];
			if (WIFEXITED(status)) {
            	printf("Exit status: %d\n", WEXITSTATUS(status));
        	} else {
            	printf("Error\n");
        	}
		}
	}

}