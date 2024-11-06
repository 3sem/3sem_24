#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include "common_func.h"
#include "fifo_func.h"

ssize_t file_to_file(int in_fd, char *interm_buff, int from_fd, const size_t size);

int fifo_init(pipe_st *pipe, const char *fifo_name, const size_t buff_size)
{
    assert(pipe);
    assert(fifo_name);

    int err_num = mkfifo(fifo_name, 0666 | O_CREAT);
    RETURN_ERROR_ON_TRUE(err_num == -1, -1,
        perror("fifo file making error"););

    int fifo_d = open(fifo_name, O_RDWR);
    RETURN_ERROR_ON_TRUE(fifo_d == -1, -1,
        perror("fifo open error occured\n"););

    char *buff = (char *)calloc(buff_size, sizeof(char));
    RETURN_ERROR_ON_TRUE(!buff, -1,
        printf("> buffer memory allocation error\n"););

    pipe->fifo_name = fifo_name;
    pipe->fifo_d    = fifo_d;
    pipe->buff      = buff;
    pipe->buff_size = buff_size;
    
    return 0;
}

void fifo_destr(pipe_st *pipe)
{
    assert(pipe);

    
    close(pipe->fifo_d);
    free(pipe->buff);

    if (pipe->fifo_name)
    {
        unlink(pipe->fifo_name);
        pipe->fifo_name = NULL;
    }

    pipe->fifo_d    = 0;
    pipe->buff      = NULL;

    return;
}

ssize_t file_to_file(int in_fd, char *interm_buff, int from_fd, const size_t size)
{
    assert(interm_buff);

    ssize_t err_num = read(from_fd, interm_buff, size);
    RETURN_ERROR_ON_TRUE(err_num == -1, -1,
        perror("read from file error\n"););

    err_num = write(in_fd, interm_buff, size);
    RETURN_ERROR_ON_TRUE(err_num == -1, -1,
        perror("write into file error\n"););

    return err_num;
}

int send_file_w_fifo(pipe_st *pipe, const char *file_name)
{
    assert(pipe);
    assert(file_name);
    
    off_t   file_size   = 0;
    
    int fd = open(file_name, O_RDONLY);
    RETURN_ERROR_ON_TRUE(fd == -1, -1,
        perror("file opening error"););

    file_size = get_file_size(fd);
    RETURN_ERROR_ON_TRUE(file_size == -1, -1);

    size_t bytes_left       = (size_t)file_size;
    RETURN_ERROR_ON_TRUE(write(pipe->fifo_d, &file_size, sizeof(size_t)) == -1, -1,
        perror("size write error\n"););

    while (bytes_left != 0)
    {
        size_t bytes_to_send = bytes_left > pipe->buff_size ? pipe->buff_size : bytes_left;

        LOG("> bytes to send: %ld\n", bytes_to_send);
        RETURN_ERROR_ON_TRUE(file_to_file(pipe->fifo_d, pipe->buff, fd, bytes_to_send) == -1, -1);

        bytes_left -= bytes_to_send;
    }

    close(fd);

    return 0;
}


int rcv_file_w_fifo(const char *rcv_file_name, pipe_st *pipe)
{
    assert(rcv_file_name);
    assert(pipe);

    int fd = open(rcv_file_name, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    RETURN_ERROR_ON_TRUE(fd == -1, -1,
        perror("rcv file opening error"););

    size_t  bytes_left  = 0;

    RETURN_ERROR_ON_TRUE(read(pipe->fifo_d, &bytes_left, sizeof(size_t)) == -1, -1,
        perror("file size read error\n"););
    LOG("> bytes left to read: %ld\n", bytes_left);

    while (bytes_left != 0)
    {
        size_t bytes_to_read = bytes_left > pipe->buff_size ? pipe->buff_size : bytes_left;

        LOG("> bytes need to read: %ld\n", bytes_left);
        RETURN_ERROR_ON_TRUE(file_to_file(fd, pipe->buff, pipe->fifo_d, bytes_to_read) == -1, -1);

        bytes_left -= bytes_to_read;
    }

    close(fd);

    return fd;
}