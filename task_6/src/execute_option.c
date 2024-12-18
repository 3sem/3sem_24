#include <stdio.h>
#include <assert.h>
#include "options.h"
#include "debugging.h"
#include "deamon.h"
#include "interactive_mode.h"


int parse_pid(const char *pid_line);

void    show_help();

int execute_option(char const *argv[])
{
    assert(argv);

    int pid       = 0;

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
        return run_deamon(pid);
    
    case INTERACTIVE:
    case INTERACTIVE_IMP:
        return run_interactive(pid);

    case HELP:
        show_help();
        break;

    default:
        LOG("[error]> non-existent option code\n");
        return NON_EXIST_OPT;
    }

    return 0;
}

int parse_pid(const char *pid_line)
{
    assert(pid_line);

    int pid = 0;
    if ((!sscanf(pid_line, "%d", &pid)) || (pid <= 0))
        return ERR_PID;    

    return pid;
}

void show_help()
{
    printf(HELP_MESSAGE);

    return;
}