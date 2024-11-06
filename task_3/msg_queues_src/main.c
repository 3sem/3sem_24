#include <stdio.h>
#include <sys/wait.h>
#include "common_func.h"
#include "mq_func.h"

int main(int argc, char const *argv[])
{
    RETURN_ERROR_ON_TRUE(argc != 3, 101,
    printf("\033[1;31m> Not enough arguments. put file to recieve, file to send\033[0m\n"););

    /*Создаём ключ, открываем очередь сообщений
    структура: ключ, mqid, 
    */

    mq_st msg_queue = {};
    int   err_num   = mq_st_create(&msg_queue, KEY_NAME);
    RETURN_ERROR_ON_TRUE(err_num, err_num);

    pid_t pid = fork();
    RETURN_ERROR_ON_TRUE(pid == -1, -1,
        perror("fork creation error"););

    if (pid)
    {
        start_timer();
        err_num = send_file_w_mq(&msg_queue, argv[2]);
        PRINT_TIME("> time to send a file with message queues: %lld\n", stop_timer());

        wait(NULL);
    }
    else
    {
        int fd = rcv_file_w_mq(argv[1], &msg_queue);
        RETURN_ERROR_ON_TRUE(fd == -1, -1,
            close(fd);
            mq_st_destr(&msg_queue););

        close(fd);
    }
    
    
    mq_st_destr(&msg_queue);
    return err_num;
}
