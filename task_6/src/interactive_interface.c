#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <limits.h>
#include <fcntl.h>
#include <signal.h>
#include "interacrtive_interface.h"
#include "interface_commands.h"
#include "debugging.h"
#include "parameters_changing.h"
#include "sig_handlers.h"
#include "functional_part.h"

int interact_with_user(const int fd);

int interface_process(const int fd, const pid_t child_pid);

int run_interactive(const pid_t pid_to_monitor)
{
    pid_t pid        = 0;

    int fd = create_ipc_fifo();
    RETURN_ON_TRUE(fd == -1, -1, destruct_ipc_fifo(););

    pid = fork();
    RETURN_ON_TRUE(pid == -1, -1,
        perror("fork error\n"););

    if (pid)
        return interface_process(fd, pid);
    else
        return functional_process(pid_to_monitor, fd);
    
    
    return 0;
}

int interface_process(const int fd, const pid_t child_pid)
{
    RETURN_ON_TRUE(signal_handler_set(interface_sigchld, SIGCHLD) == -1, -1);
    RETURN_ON_TRUE(signal_handler_set(interface_sigint,  SIGINT)  == -1, -1);

    int running_flag = 0;
    while (!running_flag)
        running_flag = interact_with_user(fd);

    int wstatus = 0;
    end_programm(child_pid);
    waitpid(child_pid, &wstatus, 0);

    close(fd);
    destruct_ipc_fifo();
    return WEXITSTATUS(wstatus);
}

int interact_with_user(const int fd)
{
    int choosen_option = 0;

    RETURN_ON_TRUE(check_interface_signals(), 1);

    printf("processmon: your programm is running in interactive mode\n");
    printf("[1] - change proccess for monitoring\n");
    printf("[2] - change the frequency of how often monitoring will happen\n");
    printf("[3] - change the temporary folder path\n");
    printf("[4] - end the programm\n");
    printf("[5] - save current parameters to standard config\n");
    printf(">");

    while (1)
    {
        choosen_option = read_number_from_input();
        if ((1 <= choosen_option && choosen_option <= 5) || choosen_option == -1)
            break;
        
        printf("Input error, please enter a number beetwen 1 and 5\n");
    }
    
    switch (choosen_option)
    {
    case -1: return 1;

    case 1: return change_pid(fd);

    case 2: return change_period(fd);

    case 3: return change_tmp_directory(fd);

    case 4: return 1;

    case 5: 
        printf("Processmon: saving config. Wait for 1 monitor period in order for config to apply\n");
        return change_config(fd, SAVE_CFG, NULL, 0);
        
    default:
        LOG("[error]> NON-existent option chosen\n");
        return NON_EXIST_OPT;
    }

    return 0;
}