#include "full_duplex.h"

#include <stdio.h>
#include <unistd.h>

ssize_t recieve(Pipe* self, bool is_parent, char* buf, size_t max_len)
{
	ssize_t read_amount = 0;
	if (is_parent) 	read_amount = read(self->fd_back[0],   buf, max_len);
	else 			read_amount = read(self->fd_direct[0], buf, max_len);

	return read_amount;
}

ssize_t send(Pipe* self, bool is_parent, const char* msg, size_t msg_len)
{
	ssize_t write_amount = 0;
	if (is_parent) 	write_amount = write(self->fd_direct[1], msg, msg_len);
	else			write_amount = write(self->fd_back[1],   msg, msg_len);

	return write_amount;
}


int main()
{
	Ops op_table = {recieve, send};
	fprintf(stderr, "%p %p\n", recieve, send);

	size_t buf_size = 10;
	Pipe* pipe = pipe_ctor(&op_table, buf_size);
	if (pipe == NULL)
	{
		fprintf(stderr, "bad pipe ctor\n");
		exit(-1);
	}

	fprintf(stderr, "pipe created\n");

	pid_t pid = fork();

	bool is_parent = true;
	size_t bb_size = 100;
	if (pid > 0)
	{
		fprintf(stderr, "parent executing\n");

		char* big_buffer = (char*)calloc(bb_size, sizeof(char));
		if(!big_buffer)
		{
			perror("bb calloc");
			exit(-1);
		}

		for (size_t id = 0; id < bb_size; ++id)
		{
			big_buffer[id] = 'r';
		}

		pipe->actions.send(pipe, is_parent, big_buffer, bb_size);
		free(big_buffer);

		char buf[100] = {};
		pipe->actions.recieve(pipe, is_parent, buf, sizeof(buf) - 1);

		printf("from child to parent: %s\n", buf);
	}
	else if (pid == 0)
	{
		fprintf(stderr, "child executing\n");

		pipe->actions.send(pipe, !is_parent, "hi parent", sizeof("hi parent"));

		char* big_buffer = (char*)calloc(bb_size, sizeof(char));
		if(!big_buffer)
		{
			perror("bb calloc");
			exit(-1);
		}

		pipe->actions.recieve(pipe, !is_parent, big_buffer, bb_size);

		big_buffer[bb_size - 1] = '\0';

		printf("from parent to child: %s\n", big_buffer);
		free(big_buffer);
	}
	else
	{
		printf("bad fork\n");
	}


	return 0;

}
