#include <stdio.h>
#include <sys/time.h>
#include <sys/wait.h>
#include "fifo_func.h"
#include "common_func.h"

int main(int argc, char const *argv[])
{
    RETURN_ERROR_ON_TRUE(argc != 3, 101,
        printf("\033[1;31m> Not enough arguments. put file to recieve, file to send\033[0m\n"););

    pipe_st         pipe    = {};
    int             err_num = fifo_init(&pipe, FIFO_NAME, BUFF_SIZE);
    RETURN_ERROR_ON_TRUE(err_num, err_num);

    pid_t pid = fork();
    RETURN_ERROR_ON_TRUE(pid == -1, -1,
        perror("fork creation error"););

    if (pid)
    {
        start_timer();
        err_num = send_file_w_fifo(&pipe, argv[2]);
        PRINT_TIME("> time to send a file with shared memory: %lld\n", stop_timer());

        wait(NULL);
    }
    else
    {

        int fd = rcv_file_w_fifo(argv[1], &pipe);
        RETURN_ERROR_ON_TRUE(fd == -1, -1,
            fifo_destr(&pipe););

        close(fd);
    }
    
    
    fifo_destr(&pipe);
    return err_num;
}
