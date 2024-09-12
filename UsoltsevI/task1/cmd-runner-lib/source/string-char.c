//--------------------------------------------------------------------
//
// string-char implementation
//
//--------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/string-char.h"

static const char *SYMBOLS = "!()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[]^_`abcdefghijklmnopqrstuvwxyz{|}~";
static const char *DELIMS = " ,\n\t";

int constains_char(const char *str, char c) {
    if (str == NULL) {
        return 0;
    }

    for (int i = 0; str[i] != '\0'; ++i) {
        if (str[i] == c) {
            return 1;
        }
    }

    return 0;
}

int count_char_in_string(const char *str, char c) {
    if (str == NULL) {
        return 0;
    }

    int counter = 0;

    for (int i = 0; str[i] != 0; ++i) {
        if (str[i] == c) {
            ++counter;
        }
    }

    return counter;
}

int count_words_in_string(const char *str, int len) {
    if (str == NULL) {
        return 0;
    }

    int counter = 0;

    for (int i = 0; str[i] != '\0' && i < len; ++i) {
        if (i == 0 && constains_char(SYMBOLS, str[i])) {
            ++counter;
        } else {
            if (constains_char(DELIMS, str[i - 1]) && constains_char(SYMBOLS, str[i])) {
                ++counter;
            }
        }
    }

    return counter;
}

char *get_next_word(const char *str, int *end_index) {
    if (str == NULL) {
        return 0;
    }

    int start = 0;
    int end = 0;

    while (str[start] != '\0' && constains_char(DELIMS, str[start])) {
        ++start;
    }

    if (str[start] == '\0') {
        return NULL;
    }

    end = start;

    while (str[end] != '\0' && !constains_char(DELIMS, str[end])) {
        ++end;
    }

    char *result = (char *) calloc(end - start + 1, sizeof(char));
    *end_index = end;
    
    memmove(result, &str[start], (end - start) * sizeof(char));

    return result;
}

const char *end_string(const char *str) {
    if (str == NULL) {
        return NULL;
    }

    const char *ptr = str;

    while (*ptr != '\0') {
        ++ptr;
    }

    return ptr;
}
