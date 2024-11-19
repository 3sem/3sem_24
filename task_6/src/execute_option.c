#include <stdio.h>
#include <assert.h>
#include "options.h"
#include "debugging.h"

int     run_deamon();

int     run_interactive();

void    show_help();


int execute_option(const char *option_line)
{
    assert(option_line);

    unsigned int option = parse_options(option_line);
    RETURN_ON_TRUE(option == ERR_OPT, ERR_OPT);

    switch (option)
    {
    case DEAMON:
        RETURN_ON_TRUE(run_deamon(), 0);
        break;
    
    case INTERACTIVE:
        RETURN_ON_TRUE(run_interactive(), 0);
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

int run_interactive()
{
    return 0;
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