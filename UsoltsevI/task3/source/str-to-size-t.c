//-----------------------------------------------------------------
//
// str-to-size-t implementation
//
//-----------------------------------------------------------------

#include <stdio.h>
#include "../include/str-to-size-t.h"

size_t str_to_size_t(const char *str) {
    int i = 0;
    size_t result = 0;

    while (str[i] != '\0') {
        ++i;
    }

    --i;
    size_t mul = 1;

    while (i >= 0 && str[i] <= '9' && str[i] >= '0') {
        result += (str[i] - '0') * mul;
        --i;
        mul *= 10;
    }

    return result;
}