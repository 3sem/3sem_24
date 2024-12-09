#include <stdio.h>
#include <sys/time.h>
#include <sys/wait.h>
#include "shared_mem_func.h"

int main(int argc, char const *argv[])
{
    RETURN_ERROR_ON_TRUE(argc != 3, 101,
        printf("\033[1;31m> Not enough arguments. put file to recieve, file to send\033[0m\n"););

    shm_st          sh_mem  = {};
    int             err_num = shm_init(&sh_mem, SHM_NAME, BUFF_SIZE);
    RETURN_ERROR_ON_TRUE(err_num, err_num);

    pid_t pid = fork();
    RETURN_ERROR_ON_TRUE(pid == -1, -1,
        perror("fork creation error"););

    if (pid)
    {
        RETURN_ERROR_ON_TRUE(create_mapping(&sh_mem) == -1, -1);

        start_timer();
        err_num = send_file_w_sh_memory(&sh_mem, argv[2]);
        PRINT_TIME("> time to send a file with shared memory: %lld\n", stop_timer());

        wait(NULL);
        unlink_sems();
    }
    else
    {
        RETURN_ERROR_ON_TRUE(create_mapping(&sh_mem) == -1, -1);

        int fd = rcv_file_w_sh_memory(argv[1], &sh_mem);
        RETURN_ERROR_ON_TRUE(fd == -1, -1,
            shm_destr(&sh_mem););

        close(fd);
    }
    
    
    err_num = shm_destr(&sh_mem);
    return err_num;
}
