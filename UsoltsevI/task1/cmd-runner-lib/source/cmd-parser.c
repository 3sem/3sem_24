//--------------------------------------------------------------------
//
// cmd-parser implementation
//
//--------------------------------------------------------------------

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../include/cmd-parser.h"
#include "../include/string-char.h"

static char **parse_cmd(const char *start, const char *end) {
    if (start == NULL || end == NULL) {
        return NULL;
    }

    int num_words = count_words_in_string(start, end - start);
    
    char **result = (char **) calloc(num_words, sizeof(char *));
    int end_index = 0;

    for (int i = 0; i < num_words; ++i) {
        int shift = 0;
        result[i] = get_next_word(start + end_index, end - start - end_index, &shift);
        end_index += shift;
    }

    return result;
}

char ***parse_cmds(const char *cmd) {
    if (cmd == NULL || cmd[0] == '\0') {
        return NULL;
    }

    int num_commands = count_char_in_string(cmd, '|') + 1;
    char ***result = (char ***) calloc(num_commands + 1, sizeof(char **));

    const char *border = cmd;
    const char *prev_border;

    for (int i = 0; i < num_commands; ++i) {
        prev_border = border;
        border = strchr(border, '|');

        if (border == NULL) {
            border = end_string(cmd) - 1;
        }

        result[i] = parse_cmd(prev_border, border);

        if (border != NULL) {
            ++border;
        }
    }

    return result;
}
