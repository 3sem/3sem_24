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
	ssize_t (*recieve)	(Pipe* self, bool is_parent);
	ssize_t (*send)		(Pipe* self, bool is_parent);
} Ops;

typedef struct pPipe
{
	size_t len; // data length in intermediate buffer
	char* data; // intermediate buffer

	int fd_direct[2];	// parent-->child
	int fd_back[2];		// child-->parent

	Ops actions; // op-table set by user
} Pipe;

Pipe* pipe_ctor(const Ops* _ops);


#endif //FULL_DUPLEX_PIPE_H
