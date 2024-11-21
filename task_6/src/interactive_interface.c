#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <limits.h>
#include <fcntl.h>
#include <signal.h>
#include "interacrtive_interface.h"
#include "debugging.h"
#include "config_changing_funcs.h"
#include "memory_watcher.h"
#include "sig_handlers.h"

unsigned int read_number_from_input();

int interact_with_user(const int fd, const pid_t child_pid);

int interface_process(const int fd, const pid_t child_pid);

int functional_process(const pid_t pid_to_monitor, const int fd);

int run_interactive(const pid_t pid_to_monitor)
{
    pid_t pid        = 0;

    int fd = create_cfg_fifo();
    RETURN_ON_TRUE(fd == -1, -1, destruct_cfg_fifo(););

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
    //Поставить обработчик сигналов на процесс, улучшить интерфейс визуально

    int running_flag = 0;
    while (!running_flag)
        running_flag = interact_with_user(fd, child_pid);

    int wstatus = 0;
    waitpid(child_pid, &wstatus, 0);

    close(fd);
    destruct_cfg_fifo();
    return WEXITSTATUS(wstatus);
}

int functional_process(const pid_t pid_to_monitor, const int fd)
{
    struct sigaction new_action = {};
    new_action.sa_handler = functional_process_sigint;
    new_action.sa_flags = (int)SA_RESETHAND;

    sigaction(SIGINT, &new_action, NULL);

    config_st config = {pid_to_monitor, STANDART_PERIOD, STANDART_FILE_OUTPUT};
    int ret_val = 0;

    while (1)
    {
        RETURN_ON_TRUE(check_if_signaled(), 1);

        sleep(config.period);

        ret_val = update_config(&config, fd);
        //Здесь функция основного функционала
        printf("config is: {\"%d\", \"%u\", \"%d\"}\n", config.monitoring_pid, config.period, config.diff_file_fd);

        if (ret_val)
            break;
    }

    close(config.diff_file_fd);
    close(fd);
    
    return ret_val;
}

int interact_with_user(const int fd, const pid_t child_pid)
{
    unsigned int choosen_option = 0;
    int error = 0;

    printf("\033[47;30m\nprocessmon: your programm is running in interactive mode\n");
    printf("[1] - change proccess for monitoring\n");
    printf("[2] - change the frequency of how often monitoring will happen\n");
    printf("[3] - change the output file\n");
    printf("[4] - end the programm\n");

    while (1)
    {
        choosen_option = read_number_from_input();
        if (1 <= choosen_option && choosen_option <= 4)
            break;
        
        printf("Input error, please enter a number beetwen 1 and 4\n");
    }
    
    switch (choosen_option)
    {
    case 1:
    {
        LOG("> changing the pid\n");
        pid_t new_pid = (pid_t)read_number_from_input();
        error = change_config(fd, PID, &new_pid, sizeof(pid_t));
        RETURN_ON_TRUE(error == -1, -1);
        break;
    }

    case 2:
    {
        LOG("changing the timing\n");
        unsigned int new_period = read_number_from_input();
        error = change_config(fd, PERIOD, &new_period, sizeof(unsigned int));
        RETURN_ON_TRUE(error == -1, -1);
        break;
    }

    case 3:
    {
        LOG("changing a filepath\n");
        char path[PATH_MAX] = {};
        scanf("%s", path);
        int output_fd = open(path, 0666 | O_CREAT);
        RETURN_ON_TRUE(output_fd == -1, -1, perror("new output file creation error\n"););

        error = change_config(fd, DIFF_FILE_FD, &output_fd, sizeof(int));
        RETURN_ON_TRUE(error == -1, -1);
        break;
    }

    case 4:
        printf("\033[0m\n");
        kill(child_pid, SIGINT);
        return 1;

    default:
        LOG("[error]> NON-existent option chosen\n");
        return NON_EXIST_OPT;
    }

    return 0;
}

unsigned int read_number_from_input()
{
    unsigned int number  = 0;
    int scanned = 0;

    while (1)
    {   
        scanned = scanf("%u", &number);
        if (scanned)
            break;
        
        printf("Input error, please enter a number\n");
        while (getchar() != '\n');
    }

    return number;
}