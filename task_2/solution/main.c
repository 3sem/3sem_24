#include "full_duplex.h"
#include "big_data_transfer.h"

#include <stdio.h>
#include <unistd.h>

ssize_t recieve(Pipe* self, bool is_parent)
{
	ssize_t read_amount = 0;

	if (is_parent) 	read_amount = read(self->fd_back[0],   self->data, self->len);
	else 			read_amount = read(self->fd_direct[0], self->data, self->len);

	return read_amount;
}

ssize_t send(Pipe* self, bool is_parent)
{
	ssize_t write_amount = 0;

	if (is_parent) 	write_amount = write(self->fd_direct[1], self->data, self->len);
	else			write_amount = write(self->fd_back[1],	 self->data, self->len);

	return write_amount;
}

int main(int argc, char** argv)
{
	Ops op_table = {recieve, send};
	fprintf(stderr, "%p %p\n", recieve, send);

	Pipe* pipe = pipe_ctor(&op_table);
	if (pipe == NULL)
	{
		fprintf(stderr, "bad pipe ctor\n");
		exit(-1);
	}
	fprintf(stderr, "pipe created\n");

	pid_t pid = fork();

	bool is_parent = true;

	if (pid > 0)
	{
		fprintf(stderr, "parent executing\n");

		send_big_data(argv[1], pipe, is_parent);
	}
	else if (pid == 0)
	{
		fprintf(stderr, "child executing\n");

		recieve_big_data(argv[2], pipe, !is_parent);
	}
	else
	{
		printf("bad fork\n");
	}

	return 0;
}
