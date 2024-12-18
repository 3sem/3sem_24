#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "options.h"
#include "debugging.h"

unsigned int parse_options(const char *line)
{
    assert(line);

    RETURN_ON_TRUE(!strcmp(line, "-d"), DEAMON,         LOG("> deamon option found\n"););

    RETURN_ON_TRUE(!strcmp(line, "-i"), INTERACTIVE,    LOG("> interactive option found\n"););

    RETURN_ON_TRUE(!strcmp(line, "-h"), HELP,           LOG("> help option found\n"););

    int num = 0;
    int n   = 0;

    sscanf(line, "%d%n", &num, &n);
    RETURN_ON_TRUE(n,                   INTERACTIVE_IMP,    LOG("> implicit interactive option found\n"););
    
    printf(INVALID_ARG_ERR);
    return ERR_OPT;
}

