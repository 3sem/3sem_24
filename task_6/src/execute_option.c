#include <stdio.h>
#include <assert.h>
#include "options.h"
#include "debugging.h"
#include "interacrtive_interface.h"

int     run_deamon();

pid_t   parse_pid(const char *pid_line);

void    show_help();

int execute_option(char const *argv[])
{
    assert(argv);

    pid_t pid       = 0;

    unsigned int option = parse_options(argv[1]);
    RETURN_ON_TRUE(option == ERR_OPT, ERR_OPT);

    if (option == INTERACTIVE_IMP)
        pid = parse_pid(argv[1]);
    else 
        pid = parse_pid(argv[2]);
    RETURN_ON_TRUE(pid == ERR_PID, ERR_PID);
    LOG("> PID to monitor is: %d\n", pid);

    switch (option)
    {
    case DEAMON:
        RETURN_ON_TRUE(run_deamon(), 0);
        break;
    
    case INTERACTIVE:
    case INTERACTIVE_IMP:
        RETURN_ON_TRUE(run_interactive(pid), 0);
        break;

    case HELP:
        show_help();
        break;

    default:
        LOG("[error]> non-existent option code\n");
        return NON_EXIST_OPT;
    }

    return 0;
}

int run_deamon()
{
    return 0;
}

pid_t parse_pid(const char *pid_line)
{
    assert(pid_line);

    pid_t pid = 0;
    if (!sscanf(pid_line, "%d", &pid))
        return ERR_PID;

    return pid;
}

void show_help()
{
    printf("processmon is a service to actively monitor process's changes in memory mapping\n");
    printf("Usage: processmon [option] PID_number\n");
    printf("Given options are:\n");
    printf("[-d]: run programm in the background\n");
    printf("[-i]: run programm in interactive mode\n");
    printf("[-h]: show this menu\n");
    printf("Note: there can be only 1 option. If no option is given, process runs in interactive mode\n");
    printf("Interacting in the background:\n");
    printf("\"chpid PID_number\" - change id of a process that is being monitored\n");
    printf("\"chprd TIME\" - change the period of how frequently monitoring will happen\n");

    return;
}