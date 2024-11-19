#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include "interacrtive_interface.h"
#include "debugging.h"
#include "memory_watcher.h"

//Создаём дочерний процесс, который будет работать в фоне
//Убийство процесса будет происходить через сигнал
//Взаимодействие с процессом будет происходить через pipe

int run_interactive()
{
    int     pipe_fd[2] = {0};
    pid_t   pid        = 0;

    RETURN_ON_TRUE(pipe(pipe_fd) == -1, -1, LOG("pipe creation error\n"););
    pid = fork();
    RETURN_ON_TRUE(pid == -1, -1,
        perror("fork error\n"););

    if (pid)
    {
        int running_flag = 0;
        while (running_flag != 1)
        {
            interact_with_user();
        }
    }
    else
    {

    }
    
    
    return 0;
}

int interact_with_user()
{
    int number = 0;

    printf("\033[46mprocessmon: your programm is running in interactive mode\033[0m\n");
    printf("\033[46m[1] - change proccess for monitoring\033[0m\n");
    printf("\033[46m[2] - change the frequency of how often monitoring will happen\033[0m\n");
    printf("\033[46m[3] - change the output file\033[0m\n");

    scanf("%d", &number);
}
