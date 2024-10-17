#include <stdio.h>
#include <string.h>

#include "input.h"
#include "logger.h"

int input_cmd(char *cmd_ptr, size_t nbyte) {
    PRINT_LOG("Waiting for command...\n");

    if (fgets(cmd_ptr, nbyte, stdin) == NULL) {
        return -1;
    }

    size_t len = strlen(cmd_ptr);
    if (len > 0 && cmd_ptr[len - 1] == '\n') {
        cmd_ptr[len - 1] = '\0';
    }

    return 0;
}
