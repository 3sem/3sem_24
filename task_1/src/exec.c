#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "exec.h"

#define MAX_PATH_LEN 1024

const char* BIN_DIRS[] = {"/bin/", "/usr/bin/"};

static int execute(Command* command, int read_desc, int write_desc);
static int execute_terminal_command(Command* command);

int execute_commands(CommandsArray* commands_array) {
    assert(commands_array);

    int first_pipe[2]  = {0, 1};  // current pipe for proccess link (set it to (stdin, stdout))
    int second_pipe[2] = {0, 1};

    for (size_t current_number = 0; current_number < commands_array->commands_count; current_number++) {
        Command* current_command = &(commands_array->array[current_number]);

        if (execute_terminal_command(current_command)) continue;

        if (get_splitter(current_command)      == PIPE_COMMAND_SPLITTER &&
            get_prev_splitter(current_command) != PIPE_COMMAND_SPLITTER) {
            pipe(second_pipe);
        } else if (get_splitter(current_command)      == PIPE_COMMAND_SPLITTER &&
                   get_prev_splitter(current_command) == PIPE_COMMAND_SPLITTER) {
            pipe(first_pipe);
            pipe_swap(first_pipe, second_pipe);
        }

        const pid_t child_pid = fork();
        if (child_pid < 0) { assert(0); }  // fork error

        if (child_pid) {
            if (get_splitter(current_command)      == PIPE_COMMAND_SPLITTER) {
                close(second_pipe[1]);
            } 

            int status = 0;
            waitpid(child_pid, &status, 0);
        } else {
            int current_read_desc  = 0;
            int current_write_desc = 1;

            if (get_prev_splitter(current_command) != PIPE_COMMAND_SPLITTER && 
                get_splitter(current_command)      == PIPE_COMMAND_SPLITTER) {
                current_write_desc = second_pipe[1];
                current_read_desc = 0;
                close(second_pipe[0]);
            } else if (get_prev_splitter(current_command) == PIPE_COMMAND_SPLITTER &&
                       get_splitter(current_command)      != PIPE_COMMAND_SPLITTER) {
                current_read_desc = second_pipe[0];
                current_write_desc = 1;
                close(second_pipe[1]);
            } else if (get_prev_splitter(current_command) == PIPE_COMMAND_SPLITTER &&
                       get_splitter(current_command)      == PIPE_COMMAND_SPLITTER) {
                current_read_desc  = first_pipe[0];
                current_write_desc = second_pipe[1];
                close(first_pipe[1]);
                close(second_pipe[0]);
            }

            execute(current_command, current_read_desc, current_write_desc);
        }
    }

    return 0;
}

static int execute(Command* command, int read_desc, int write_desc) {
    assert(command);

    char** args = (char**) calloc(command->argument_count + 1, sizeof(char*));
    assert(args);

    args[0] = command->command;
    for (size_t i = 0; i < command->argument_count; i++) {
        args[i + 1] = command->argument_list[i];
    }

    dup2(write_desc, 1);
    dup2(read_desc,  0);

    for (size_t i = 0; i < sizeof(BIN_DIRS) / sizeof(char*); i++) {
        char path[MAX_PATH_LEN] = {};
        strcat(path, BIN_DIRS[i]);
        strcat(path, command->command);

        execv(path, args);
    }

    fprintf(stderr, "Command doesn't exist\n");

    free(args);

    return 0;
}

static int execute_terminal_command(Command* command) {
    assert(command);

    if (!strcmp(command->command, "exit")) {
        exit(0);
        return 1;
    }
    else if (!strcmp(command->command, "cd")) {
        if (!command->argument_count) return 1;

        chdir(command->argument_list[0]);

        return 1;
    }

    return 0;
}