#ifndef SHELL_H_
#define SHELL_H_

#include "token.h"

#define MAX_STR_SIZE 1024

CmdArray input_cmds();

int* executer( CmdArray cmd_arr);


void print_exit_codes( int* exit_codes,
                       CmdArray cmd_array);

#endif
