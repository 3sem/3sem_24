#ifndef DUPLEX_PIPE_H
#define DUPLEX_PIPE_H

#include <stddef.h>

typedef struct _Pipe    Pipe;
typedef struct _OpTable OpTable;

typedef size_t (*receive_t)(Pipe *self);
typedef size_t (*send_t)   (Pipe *self);

typedef struct _OpTable
{
    receive_t receive;
    send_t    send;

} OpTable;

typedef struct _Pipe
{
    char*  data;
    size_t dataSize;
    size_t dataCapacity;

    int fdForw[2];
    int fdBack[2];

    OpTable ops;

} Pipe;

Pipe* CreateDuplexPipe  (OpTable ops);

int   DestroyDuplexPipe (Pipe* dpipe);

#endif // DUPLEX_PIPE_H
