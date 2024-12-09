#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
#include "log.h"

#ifndef PIPE_RUN
#define PIPE_RUN

void exec_cmd(char ***cmd);

#endif