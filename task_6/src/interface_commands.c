#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>
#include <limits.h>
#include <string.h>
#include "config_changing_funcs.h"
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
    unsigned int new_period = read_number_from_input();
    printf("New period is: %u. Wait for the period to apply\n", new_period);
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