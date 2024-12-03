#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>
#include <limits.h>
#include <string.h>
#include "parameters_changing.h"
#include "debugging.h"
#include "interface_commands.h"

int change_pid(const int fd)
{
    LOG("> changing the pid\n");
    printf("\bChanging the PID. Please enter new process ID:\n> ");
    pid_t new_pid = (pid_t)read_number_from_input();
    printf("New PID is: %d", new_pid);
    return change_config(fd, PID, &new_pid, sizeof(pid_t));
}

int change_period(const int fd)
{
    LOG("changing the timing\n");
    printf("\bChanging monitoring period. Please enter new time in seconds:\n>");
    int new_period = 0;
    while (1)
    {
        new_period = read_number_from_input();
        if (new_period <= 0)
        {
            printf("Processmon: period you entered can't be set, please enter a positive number\n");
            continue;
        }
        
        break;
    }
    
    printf("New period is: %u. Wait for the period to apply\n", (unsigned int)new_period);
    return change_config(fd, PERIOD, &new_period, sizeof(unsigned int));
}

int change_filepath(const int fd)
{
    LOG("changing a filepath\n");
    printf("\bChanging output file. Please enter new file path:\n>");
    char path[PATH_MAX] = {};
    scanf("%s", path);
    printf("New file path is: \"%s\"", path);


    return change_config(fd, DIFF_FILE_FD, path, strlen(path) * sizeof(char));

    int output_fd = open(path, O_CREAT, 0777);
    RETURN_ON_TRUE(output_fd == -1, -1, perror("new output file creation error"););
}

int end_programm(const int child_pid)
{
    printf("Finishing programm.\033[0m\n\n");
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
        RETURN_ON_TRUE(scanned == -1, -1, perror("input number read error"););
        if (scanned)
            break;
        
        printf("Input error, please enter a number\n");
        while (getchar() != '\n');
    }

    return number;
}