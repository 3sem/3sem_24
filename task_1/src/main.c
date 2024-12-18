#include <stdio.h>
#include <limits.h>
#include "parser.h"
#include "run_pipe.h"
#include "read_cmd.h"
#include "debug.h"


int main()
{
    _OPEN_LOG("console_emulator.log");
    
    char cmd_string [_POSIX_ARG_MAX]               = {0};
    int error                       = 0;
    unsigned int commands_number    = 0;

    error = read_cmd(cmd_string);
    if (error)
    {
        LOG_ERR("input reading error");
        _CLOSE_LOG();
        return error;
    }

    char ***commands = parse_string(cmd_string, &commands_number);
    if (!commands)
    {
        LOG_ERR("parsing error occured");
        _CLOSE_LOG();
        return PARSE_ERR;
    }

    exec_cmd(commands);

    clear_buffers(commands, commands_number);
    _CLOSE_LOG();
    return 0;
}