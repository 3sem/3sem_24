/* You need something like this */
#include <stdio.h>
#include <unistd.h>

#ifndef DUPLEX_CHANNEL
#define DUPLEX_CHANNEL

#define INTERM_BUFF_SIZE       4096

#define LOG(...)

typedef struct pPipe channel_t;

typedef struct op_table Ops;

typedef struct op_table  
{
    void (* init_child_channel)(channel_t *self);
    void (* init_parent_channel)(channel_t *self);

    ssize_t (*send)(channel_t *self, int fd);
    ssize_t (*recieve)(channel_t *self, int fd);
} Ops;

typedef struct pPipe 
{
    char* data; // intermediate buffer
    size_t len; // data length in intermediate buffer
    int fd_direct[2]; // array of r/w descriptors for "pipe()" call (for parent-->child direction)
    int fd_back[2]; // array of r/w descriptors for "pipe()" call (for child-->parent direction)
    
    Ops actions;
} channel_t;

ssize_t send(channel_t *self, int fd);

ssize_t recieve(channel_t *self, int fd);

void init_child_channel(channel_t *self);

void init_parent_channel(channel_t *self);

void destructor(channel_t *self);

channel_t *constructor();

#define RETURN_ERROR_ON_TRUE(statement, return_val, ...)                \
    do                                                                  \
    {                                                                   \
        if (statement)                                                  \
        {                                                               \
            __VA_ARGS__                                                 \
            return return_val;                                          \
        }                                                               \
    } while (0)
    
    


#endif