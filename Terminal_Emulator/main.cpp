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

const size_t MAX_ARG_AMOUNT = 100;

void parse_cmd(char** args)
{
	char delim[]        = " \n";
	char cmd[ARG_MAX]   = {};

	scanf("%[^\n]", cmd);
	// printf("read cmd: %s\n", cmd);

	// printf("read args:\n");
	size_t id = 0;
	for (args[id] = strtok(cmd, delim); args[id] != NULL; args[id] = strtok(NULL, delim))
	{
		// puts(args[id]);
		++id;
	}

	// sprintf(args[id], " > %s", fds[1]);
	// puts(args[id]);
}

void run_cmd()
{
	int fds[2] = {};

	if(pipe(fds) == -1)
	{
		perror("pipe");
		exit(EXIT_FAILURE);
	}

	const pid_t pid = fork();
	if (pid < 0)
	{
		exit(1);
	}

	if (pid)
	{
		int status = 0;
		waitpid(pid, &status, 0);

		printf("ret code: %d\n", WEXITSTATUS(status));

		return;
	}
	else
	{
		char* args[MAX_ARG_AMOUNT] = {};
		parse_cmd(args);

		assert(args);

		execvp(args[0], args);

		perror("exec error");
		exit(EXIT_FAILURE);
	}
}

int main()
{
	while (true)
	{
		run_cmd();
	}

	return 0;
}
