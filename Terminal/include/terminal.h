#ifndef TERMINAL_H
#define TERMINAL_H

#include <stdlib.h>

static const size_t MAX_ARGS_AMOUNT = 100;

int run_cmd();
int parse_cmd(char* cmd, char* args[MAX_ARGS_AMOUNT]);

#endif // TERMINAL_H
