#include "full_duplex.h"

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

Pipe* pipe_ctor(const Ops* _ops, size_t _len)
{
	Pipe* created_pipe = (Pipe*)calloc(1, sizeof(Pipe));
	if (!created_pipe)
	{
		perror("pipe calloc");
		return NULL;
	}

	created_pipe->actions = *_ops;

	created_pipe->len = _len;

	created_pipe->data = (char*)calloc(_len, sizeof(char));
	if (!created_pipe->data)
	{
		perror("data calloc\n");
		free(created_pipe);
		return NULL;
	}

	if (pipe(created_pipe->fd_direct) == -1)
	{
		perror("pipe fd_direct");

		free(created_pipe->data);
		free(created_pipe);

		return NULL;
	}
	if (pipe(created_pipe->fd_back) == -1)
	{
		perror("pipe fd_back");
		
		close(created_pipe->fd_direct[0]);
		close(created_pipe->fd_direct[1]);

		free(created_pipe->data);
		free(created_pipe);
		return NULL;
	}

	return created_pipe;
}
