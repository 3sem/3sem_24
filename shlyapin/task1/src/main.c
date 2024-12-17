#include "cmd_runner.h"
#include "input.h"
#include "logger.h"

#define MAX_LEN_CMD 2048

int main() {
    char str_cmd[MAX_LEN_CMD];

    while (1) {
        if (input_cmd(str_cmd, MAX_LEN_CMD)) {
            PRINT_ERROR("Error reading command\n");
        }

        PRINT_LOG("Input: {%s}\n", str_cmd);
        
        if (run_cmd(str_cmd)) {
            PRINT_ERROR("Error running command\n");
        }
    }

    return 0;
}
