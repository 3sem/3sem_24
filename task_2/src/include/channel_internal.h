#ifndef CHANNEL_INTERNAL
#define CHANNEL_INTERNAL

#include <stdio.h>
#include <channel.h>

int send_partially(channel_t *ch, int pipe_d, int fd, const size_t file_size);

ssize_t send_fully(channel_t *ch, int pipe_d, int fd, const size_t file_size);

ssize_t write_to_interm_buff(channel_t *ch, int fd, const size_t count);

void clear_interm_buff(channel_t *ch);

off_t get_file_size(int fd);

ssize_t rcv_to_interm_buff(channel_t *self, int opened_descr);

#endif