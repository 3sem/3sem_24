#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "console.h"
#include "parser.h"
#include "exec.h"

const size_t INITIAL_CAPACITY = 5;
const size_t MAX_DIR_LEN      = 2048;

typedef struct {
    size_t argc;
    char** argv;
    size_t capacity;
} CommandLine;

static int command_line_ctor(CommandLine* cmd, size_t capacity);
static int command_line_dtor(CommandLine* cmd);
static int command_line_dump(CommandLine* cmd);
static int read_command(CommandLine* cmd, char* line);
static int command_line_realloc(CommandLine* cmd);

int console() {
    CommandLine cmd = {};
    CommandsArray commands_array = {};

    while (1) {
        command_line_ctor(&cmd, INITIAL_CAPACITY);

        char* current_dir = getcwd(NULL, MAX_DIR_LEN);

        char* line = readline(strcat(current_dir, " >> ")); // get line from console
        add_history(line);
        free(current_dir);

        read_command(&cmd, line);                               // tokenize command line
        parse_commands(cmd.argc, cmd.argv, &commands_array);    // parse command line

        // commands_array_dump(&commands_array);
        execute_commands(&commands_array);

        commands_array_dtor(&commands_array);   // Destruct all data structures of curren command line
        command_line_dtor(&cmd);
        free(line);
    }
    
    return 0;
}

static int command_line_ctor(CommandLine* cmd, size_t capacity) {
    assert(cmd);

    cmd->capacity = capacity;

    cmd->argv = (char**) calloc(capacity, sizeof(char*));
    cmd->argc = 0;

    return 0;
}

static int command_line_dtor(CommandLine* cmd) {
    assert(cmd);
    
    free(cmd->argv);
    cmd->argc     = 0;
    cmd->capacity = 0;

    return 0;
}

static int command_line_dump(CommandLine* cmd) {
    assert(cmd);

    printf( "CAPACITY: %lu\n"
            "ARGC: %lu\n"
            "ARGV: ", cmd->capacity, cmd->argc);

    for (size_t i = 0; i < cmd->argc; i++) {
        printf("%s ", cmd->argv[0]);
    }   
    putchar('\n');

    return 0;
}

static int read_command(CommandLine* cmd, char* line) {
    assert(cmd);

    char* current_token = strtok(line, " ");
    if (!current_token) return 0;

    for (size_t i = 0;; i++) {
        cmd->argv[i] = current_token;
        (cmd->argc)++;

        current_token = strtok(NULL, " ");
        if (!current_token) break;

        if (cmd->argc >= cmd->capacity - 1) {
            command_line_realloc(cmd);
        }
    }

    return 0;
}

static int command_line_realloc(CommandLine* cmd) {
    assert(cmd);

    cmd->argv = (char**) realloc(cmd->argv, cmd->capacity * sizeof(char*) * 2);
    assert(cmd->argv);

    cmd->capacity *= 2;

    return 0;
}
