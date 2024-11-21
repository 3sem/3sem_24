#include <stdio.h>
#include <signal.h>
#include "debugging.h"
#include "sig_handlers.h"

static volatile sig_atomic_t parent_sigint = 0;

void functional_process_sigint(int signum)
{
    LOG("> child process finished\n");
    parent_sigint = signum;

    return;
}

int check_if_signaled()
{
    return parent_sigint;
}