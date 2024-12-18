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

ssize_t send(channel_t *self, int fd)
{
    assert(self);

    int opened_descriptor = self->fd_back[1] == 0 ? self->fd_direct[1] : self->fd_back[1];
    LOG("opened descriptor for send is: %d between <%d> and <%d>\n", opened_descriptor, self->fd_direct[0], self->fd_back[0]);

    off_t file_size = get_file_size(fd);
    RETURN_ERROR_ON_TRUE(write(opened_descriptor, &file_size, sizeof(off_t)) == -1, -1,
        LOG("Couldn't write size of the file in channel\n"););

    return send_partially(self, opened_descriptor, fd, (size_t)file_size);
}

ssize_t recieve(channel_t *self, int fd)
{
    assert(self);

    int opened_descriptor = self->fd_back[0] == 0 ? self->fd_direct[0] : self->fd_back[0];
    LOG("opened descriptor for recieve is: %d between <%d> and <%d>\n", opened_descriptor, self->fd_direct[1], self->fd_back[1]);

    off_t recieve_size = 0;
    RETURN_ERROR_ON_TRUE(read(opened_descriptor, &recieve_size, sizeof(off_t)) != sizeof(off_t), -1,
        LOG("couldn't read size of file fully"););

    ssize_t bytes_read = 0;
    while (bytes_read != recieve_size)
    {
        bytes_read += rcv_to_interm_buff(self, opened_descriptor);

        ssize_t bytes_written = write(fd, self->data, self->len);
        RETURN_ERROR_ON_TRUE(bytes_written == -1, -1,
            perror("couldn't write to a file descriptor\n"););
        
        self->len = 0;
    }

    return bytes_read;
}

void init_child_channel(channel_t *self)
{
    assert(self);

    close(self->fd_direct[1]);
    close(self->fd_back[0]);

    self->fd_direct[1]  = 0;
    self->fd_back[0]    = 0;
    return;
}

void init_parent_channel(channel_t *self)
{
    assert(self);

    close(self->fd_direct[0]);
    close(self->fd_back[1]);

    self->fd_direct[0]  = 0;
    self->fd_back[1]    = 0;
    return;
}

void destructor(channel_t *channel)
{
    assert(channel);

    close(channel->fd_back[0]);
    close(channel->fd_back[1]);
    close(channel->fd_direct[0]);
    close(channel->fd_direct[1]);

    free(channel->data);
    channel->data          = NULL;

    free(channel);
    return;
}

channel_t *constructor()
{
    channel_t *channel = (channel_t *)calloc(1, sizeof(channel_t));
    RETURN_ERROR_ON_TRUE (!channel, NULL,
        printf("error when initialising structure\n"););

    RETURN_ERROR_ON_TRUE (pipe(channel->fd_direct) == -1, NULL,
        printf("direct pipe creation error\n");
        free(channel););

    RETURN_ERROR_ON_TRUE (pipe(channel->fd_back) == -1, NULL,
        printf("back pipe creation error\n");
        free(channel););

    LOG("opened direct descriptors are: <%d> and <%d>\n", channel->fd_direct[0], channel->fd_direct[1]);
    LOG("opened back   descriptors are: <%d> and <%d>\n", channel->fd_back[0], channel->fd_back[1]);

    channel->data = (char *)calloc(INTERM_BUFF_SIZE, sizeof(char));
    RETURN_ERROR_ON_TRUE (!channel->data, NULL,
        printf("[error]> couldn't construct pipe channel structure\n");
        free(channel););

    channel->len = 0;

    channel->actions.init_child_channel     = init_child_channel;
    channel->actions.init_parent_channel    = init_parent_channel;
    channel->actions.recieve                = recieve;
    channel->actions.send                   = send;

    return channel;
}