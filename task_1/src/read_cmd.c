#include <stdio.h>
#include <limits.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include "read_cmd.h"
#include "debug.h"

int read_cmd(char *string_ptr)
{
    assert(string_ptr);

    if (!read(STDIN_FILENO, string_ptr, sizeof(string_ptr)))
    {
        LOG("couldn't read your input");
        _CLOSE_LOG();
        return INPUT_READ_ERR;
    }

    return 0;
}