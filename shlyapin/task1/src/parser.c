#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "logger.h"

int count_char(const char *str, char chr) {
    int count = 0;
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == chr) {
            count++;
        }
    }
    return count;
}

char **parse_cmd(const char *cmd_ptr, const char *sep) {
    int nargs = count_char(cmd_ptr, sep[0]) + 1;

    char **args = (char **)calloc(nargs + 1, sizeof(char *));
    if (args == NULL) {
        PRINT_ERROR("Memory allocation failed\n");
        return NULL;
    }

    char *cmd_copy = strdup(cmd_ptr);
    if (cmd_copy == NULL) {
        PRINT_ERROR("Memory allocation failed\n");
        free(args);
        return NULL;
    }

    char *token = strtok(cmd_copy, sep);
    int i = 0;
    while (token) {
        args[i] = strdup(token);
        if (args[i] == NULL) {
            PRINT_ERROR("Memory allocation failed\n");
            free_parsed_cmd(args);
            free(cmd_copy);
            return NULL;
        }
        i++;
        token = strtok(NULL, sep);
    }
    args[i] = NULL;

    free(cmd_copy);

    for (int i = 0; args[i]; i++) {
        PRINT_LOG("Parsed argument %d: {%s}\n", i, args[i]);
    }

    return args;
}

void free_parsed_cmd(char **args) {
    if (args == NULL) return;

    for (int i = 0; args[i]; i++) {
        free(args[i]);
    }
    free(args);
}
