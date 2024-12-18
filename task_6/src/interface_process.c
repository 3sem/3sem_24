#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include "parameters_changing.h"
#include "debugging.h"
#include "interface_process.h"
#include "sig_handlers.h"

int interact_with_user(const int fd);

int change_pid(const int fd);

int change_period(const int fd);

int change_tmp_directory(const int fd);

int end_programm(const int child_pid);

int read_number_from_input();


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

    printf(INTERACTIVE_MESSAGE);

    while (1)
    {
        choosen_option = read_number_from_input();
        if ((1 <= choosen_option && choosen_option <= 7) || choosen_option == -1)
            break;
        
        printf(INPUT_ERR_MSG);
    }
    
    switch (choosen_option)
    {
    case -1: return 1;

    case 1: return change_pid(fd);

    case 2: return change_period(fd);

    case 3: return change_tmp_directory(fd);

    case 4: return 1;

    case 5: 
        printf(SAVING_CONFIG_MSG);
        return change_config(fd, SAVE_CFG, NULL, 0);

    case 6: return change_config(fd, SHOW_CFG, NULL, 0);

    case 7: return change_config(fd, CNG_SAVE_BOOL, NULL, 0);
        
    default:
        LOG("[error]> NON-existent option chosen\n");
        return NON_EXIST_OPT;
    }

    return 0;
}

int change_pid(const int fd)
{
    LOG("> changing the pid\n");
    printf(CHANGING_PID_MSG);
    pid_t new_pid = (pid_t)read_number_from_input();
    printf(NEW_PID_MSG, new_pid);
    return change_config(fd, PID, &new_pid, sizeof(pid_t));
}

int change_period(const int fd)
{
    LOG("changing the timing\n");
    printf(CHANGE_PRD_MSG);
    int new_period = 0;
    while (1)
    {
        new_period = read_number_from_input();
        if (new_period <= 0)
        {
            printf(NEW_PERIOD_ERR);
            continue;
        }
        
        break;
    }
    
    printf(NEW_PERIOD_MSG, (unsigned int)new_period);
    return change_config(fd, PERIOD, &new_period, sizeof(unsigned int));
}

int change_tmp_directory(const int fd)
{
    LOG("changing temporary directory path\n");
    printf(CHANGE_TMP_DIR);
    char path[PATH_MAX] = {};
    scanf("%s", path);
    printf(NEW_DIR_MSG, path);

    return change_config(fd, DIFF_FILE_FD, path, strlen(path) * sizeof(char));

    int output_fd = open(path, O_CREAT, 0777);
    RETURN_ON_TRUE(output_fd == -1, -1, perror("new directory path creation error"););
}

int end_programm(const int child_pid)
{
    printf(FINISHING_MSG);
    kill(child_pid, SIGINT);
    return 1;
}

int read_number_from_input()
{
    int number  = 0;
    int scanned = 0;

    while (1)
    {   
        scanned = scanf("%d", &number);
        RETURN_ON_TRUE(scanned == -1, -1);//perror("input number read error"););
        if (scanned)
            break;
        
        printf(NUM_INPUT_ERR);
        while (getchar() != '\n');
    }

    return number;
}