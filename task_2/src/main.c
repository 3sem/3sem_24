#include <stdio.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "channel.h"

int main(int argc, char const *argv[])
{
    RETURN_ERROR_ON_TRUE(argc != 4, 11, 
        printf("\033[1;31m> Please enter 3 file names: file to send, file to recieve in child and file to recieve in parent\033[0m\n"););

    char const *first_file_in_p      = argv[1];
    char const *second_file_in_p     = argv[2];
    char const *file_in_c            = argv[3];

    channel_t *channel = constructor();
    if (!channel)
        return 111;

    pid_t pid = fork();
    if      (pid == -1)
    {
        printf("[error]> fork creation error\n");
        destructor(channel);
        return 112;
    }
    else if (pid)
    {
        channel->actions.init_parent_channel(channel);

        int first_file_in_parent = open(first_file_in_p, O_RDONLY);
        RETURN_ERROR_ON_TRUE(first_file_in_parent == -1, -1, perror("first file open err\n"); destructor(channel););

        channel->actions.send(channel, first_file_in_parent);

        int second_file_in_parent = open(second_file_in_p, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
        RETURN_ERROR_ON_TRUE(second_file_in_parent == -1, -1, perror("second file open err\n"); destructor(channel););

        channel->actions.recieve(channel, second_file_in_parent);

        close(first_file_in_parent);
        close(second_file_in_parent);
        waitpid(pid, NULL, 0);

        destructor(channel);
    }
    else
    {
        channel->actions.init_child_channel(channel);

        int file_in_child = open(file_in_c, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU);
        RETURN_ERROR_ON_TRUE(file_in_child == -1, -1, perror("child file open err\n"); destructor(channel););

        channel->actions.recieve(channel, file_in_child);
        channel->actions.send(channel, file_in_child);

        close(file_in_child);

        destructor(channel);
        return 0;
    }
   
    return 0;
}
