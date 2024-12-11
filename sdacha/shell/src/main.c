#include "token.h"
#include "shell.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main()
{
    CmdArray cmd_array = input_cmds();
    
    /* for ( int i = 0; i < cmd_array.n_cmds; ++i ) */
    /* { */
    /*     for ( int n_args = 0; */
    /*           n_args < cmd_array.cmds[i].n_exec_args; */
    /*           n_args++ ) */
    /*     { */
    /*         printf("Arg %d: %s\n", */
    /*                n_args + 1, cmd_array.cmds[i].exec_args[n_args]); */
    /*     } */
    /* } */

    int* exit_codes = executer( cmd_array);

    print_exit_codes( exit_codes, cmd_array);
    free( exit_codes);

    cmd_array_destr( &cmd_array);

    return 0;
}
