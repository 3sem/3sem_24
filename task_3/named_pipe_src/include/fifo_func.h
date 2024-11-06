#ifndef FIFO_FUNCS
#define FIFO_FUNCS

typedef struct 
{
    const char *fifo_name;
    int         fifo_d;
    char       *buff;
    size_t      buff_size;
} pipe_st;

#define FIFO_NAME "./key_to_programmers_heart"

#define BUFF_SIZE 4096

int fifo_init(pipe_st *pipe, const char *fifo_name, const size_t buff_size);

void fifo_destr(pipe_st *pipe);

int send_file_w_fifo(pipe_st *pipe, const char *file_name);

int rcv_file_w_fifo(const char *rcv_file_name, pipe_st *pipe);

#endif