#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <sys/types.h>
#include <unistd.h>
#include <memory.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "channel.h"
#include "channel_internal.h"

ssize_t rcv_to_interm_buff(channel_t *self, int opened_descr)
{
    assert(self);

    ssize_t bytes_read = read(opened_descr, self->data, INTERM_BUFF_SIZE);
    LOG("<%ld> bytes were read into buffer\n", bytes_read);
    if (bytes_read == -1)
        perror("read from pipe error\n");
    else
        self->len = (size_t)bytes_read;

    return bytes_read;
}

int send_partially(channel_t *ch, int pipe_d, int fd, const size_t file_size)
{
    assert(ch);

    size_t bytes_sent = 0;

    while (file_size - bytes_sent >= INTERM_BUFF_SIZE)
    {
        assert(file_size > bytes_sent);
        RETURN_ERROR_ON_TRUE(write_to_interm_buff(ch, fd, INTERM_BUFF_SIZE) == -1, -1);

        ssize_t bytes_written = write(pipe_d, ch->data, ch->len);
        RETURN_ERROR_ON_TRUE(bytes_written == -1, -1);
        LOG("<%ld> bytes were written into the channel\n", bytes_written);
        clear_interm_buff(ch);

        bytes_sent += (size_t)bytes_written;
    }
    RETURN_ERROR_ON_TRUE(send_fully(ch, pipe_d, fd, file_size - bytes_sent) == -1, -1);
    
    return 0;
}

ssize_t send_fully(channel_t *ch, int pipe_d, int fd, const size_t file_size)
{
    assert(ch);
    assert(file_size <= INTERM_BUFF_SIZE);

    LOG("sending file fully\n");

    RETURN_ERROR_ON_TRUE(write_to_interm_buff(ch, fd, file_size) == -1, -1);

    ssize_t bytes_written = write(pipe_d, ch->data, ch->len);
    RETURN_ERROR_ON_TRUE(bytes_written == -1, -1);
    clear_interm_buff(ch);

    return bytes_written;
}

off_t get_file_size(int fd)
{
    off_t file_size = lseek(fd, 0, SEEK_END);
    RETURN_ERROR_ON_TRUE(file_size == -1, -1);
    lseek(fd, 0, SEEK_SET);

    return file_size;
}

ssize_t write_to_interm_buff(channel_t *ch, int fd, const size_t count)
{
    assert(ch);
    assert(count + ch->len <= INTERM_BUFF_SIZE);

    ssize_t bytes_read = read(fd, ch->data + ch->len, count);
    RETURN_ERROR_ON_TRUE(bytes_read == -1, -1, 
        perror("error while reading to intermediate buffer\n"););
    ch->len += count;

    LOG("<%d> bytes were written to intermediate buffer: <%s>\n", (int)count, ch->data);

    return bytes_read;
}

void clear_interm_buff(channel_t *ch)
{
    assert(ch);

    memset(ch->data, 0, INTERM_BUFF_SIZE);
    ch->len = 0;

    return;
}