#include "big_data_transfer.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <limits.h>
#include <assert.h>
#include <string.h>
#include <stdarg.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/wait.h>

#include "full_duplex.h"

const size_t chunk_size = 1000000;

void send_big_data(const char* file_name, Pipe* pipe, bool is_parent)
{
	pipe->data = (char*)calloc(chunk_size, sizeof(char));

	if (!pipe->data)
	{
		perror("bad alloc");
		exit(EXIT_FAILURE);
	}

	int fd = open(file_name, O_RDONLY);
	if (fd == -1)
	{
		perror("bad file open");
		free(pipe->data);
		exit(EXIT_FAILURE);
	}

	ssize_t count = read(fd, pipe->data, chunk_size);
	while (count)
	{
		if (count < 0)
		{
			perror("bad file read");
			break;
		}

		pipe->len = count;
		if (pipe->actions.send(pipe, is_parent) == -1)
		{
			perror("bad pipe send");
			break;
		}

		count = read(fd, pipe->data, chunk_size);
	}

	close(fd);
	free(pipe->data);
}

void recieve_big_data(const char* file_name, Pipe* pipe, bool is_parent)
{
	pipe->len = chunk_size;
	pipe->data = (char*)calloc(chunk_size, sizeof(char));

	if (!pipe->data)
	{
		perror("bad alloc");
		exit(EXIT_FAILURE);
	}

	int fd = open(file_name, O_WRONLY | O_CREAT);
	if (fd == -1)
	{
		perror("bad file open");
		free(pipe->data);
		exit(EXIT_FAILURE);
	}

	ssize_t count = pipe->actions.recieve(pipe, is_parent);
	while (count)
	{
		if (count < 0)
		{
			perror("bad write");
			break;
		}

		write(fd, pipe->data, (size_t)count);
		count = pipe->actions.recieve(pipe, is_parent);
	}

	free(pipe->data);

	close(fd);
}
