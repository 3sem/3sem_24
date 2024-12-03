#include "terminal.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <limits.h>
#include <assert.h>
#include <string.h>
#include <stdarg.h>

#include <sys/types.h>
#include <sys/wait.h>

#define DO_SAFELY(func, ...)		\
	if(func(__VA_ARGS__) == -1)		\
	{								\
		perror(#func);				\
		exit(EXIT_FAILURE);			\
	}

#define FORK(pid)					\
	pid = fork();					\
	if(pid == -1)					\
	{								\
		perror("fork");				\
		exit(EXIT_FAILURE);			\
	}

int run_cmd()
{
	char cmd[ARG_MAX] = {};

	fgets(cmd, sizeof(cmd), stdin);
	cmd[strcspn(cmd, "\n")] = '\0';

	return process(cmd, STDIN_FILENO);
}

int process(char* cmd, int in_fd)
{
    char *pipe_pos = strchr(cmd, '|');

    if (pipe_pos != NULL)
    {
        printf("pipe detected\n");

        *pipe_pos = '\0';
        char* r_cmd = ++pipe_pos;
        char* w_cmd = cmd;

        int fds[2] = {};

        DO_SAFELY(pipe, fds);
        int r_descriptor = fds[0];
        int w_descriptor = fds[1];

        pid_t w_pid = 0;
        FORK(w_pid);

        if (w_pid == 0)
        {
            printf("write child executing\n");

            DO_SAFELY(dup2, w_descriptor, STDOUT_FILENO);
            if (in_fd != STDIN_FILENO)
                DO_SAFELY(dup2, in_fd, STDIN_FILENO);

            close(fds[0]);
            close(fds[1]);

            char* args[MAX_ARGS_AMOUNT] = {};

            parse_cmd(w_cmd, args);

            DO_SAFELY(execvp, args[0], args);
        }
        else
        {
            pid_t r_pid = 0;
            FORK(r_pid);

            if (r_pid == 0)
            {
                close(fds[1]); // Закрываем дескриптор записи для дочернего процесса чтения
                process(r_cmd, r_descriptor);
            }
            else
            {
                close(fds[0]);
                close(fds[1]);

                int status = 0;
                waitpid(w_pid, &status, 0);
                printf("status: %d\n", WEXITSTATUS(status));
                waitpid(r_pid, &status, 0);
                printf("status: %d\n", WEXITSTATUS(status));
            }
        }
    }
    else
    {
        printf("no pipe\n");

        pid_t pid = 0;
        FORK(pid);

        if (pid == 0)
        {
            DO_SAFELY(dup2, in_fd, STDIN_FILENO);
            close(in_fd);

            char* args[MAX_ARGS_AMOUNT] = {};

            parse_cmd(cmd, args);

            DO_SAFELY(execvp, args[0], args);
        }
        else
        {
            int status = 0;
            waitpid(pid, &status, 0);
            printf("status: %d\n", WEXITSTATUS(status));
        }
    }

	return 0;
}


int parse_cmd(char* cmd, char* args[MAX_ARGS_AMOUNT])
{
	char delims[] = {' ', '\n'};
	char* token = strtok(cmd, delims);

	size_t id = 0;
	while(token != NULL)
	{
		// puts(token);
		args[id] = token;

		token = strtok(NULL, delims);

		++id;
	}

	fflush(stdout);

	return 0;
}
