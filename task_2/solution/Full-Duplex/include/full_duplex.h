#ifndef FULL_DUPLEX_PIPE_H
#define FULL_DUPLEX_PIPE_H

#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

typedef struct pPipe Pipe;
typedef struct op_table Ops;

typedef struct op_table
{
	ssize_t (*recieve)	(Pipe* self, bool is_parent, char* buf, size_t max_len);
	ssize_t (*send)		(Pipe* self, bool is_parent, const char* msg, size_t msg_len);
} Ops;

typedef struct pPipe
{
	size_t len; // data length in intermediate buffer
	char* data;

	int fd_direct[2];	// parent-->child
	int fd_back[2];		// child-->parent

	Ops actions;
} Pipe;

Pipe* pipe_ctor(const Ops* _ops, size_t _len);


#endif //FULL_DUPLEX_PIPE_H
