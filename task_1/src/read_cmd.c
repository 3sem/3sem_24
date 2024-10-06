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

    if (!scanf("%[^\n]", string_ptr))
    {
        LOG("couldn't read your input");
        _CLOSE_LOG();
        return INPUT_READ_ERR;
    }

    LOG("string read is: %s", string_ptr);

    return 0;
}