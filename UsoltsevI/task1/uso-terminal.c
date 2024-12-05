//--------------------------------------------------------------------
//
// uso-terminal main file
//
//--------------------------------------------------------------------

#include <stdio.h>
#include "cmd-runner.h"

static const int CMD_BUFFER_SIZE = 4096;

int main() {
    printf("\033[1;33muso-terminal$ \033[0m");

    const int size = CMD_BUFFER_SIZE;
    char cmd[size];

    while (1) {
        fgets(cmd, size, stdin);
        run_cmds(cmd);
        printf("\033[1;33muso-terminal$ \033[0m");
    }

    return 0;
}
