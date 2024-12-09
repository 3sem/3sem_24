#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <limits.h>
#include <fcntl.h>
#include <signal.h>
#include "interactive_mode.h"
#include "interface_process.h"
#include "debugging.h"
#include "parameters_changing.h"
#include "sig_handlers.h"
#include "functional_part.h"

int run_interactive(const pid_t pid_to_monitor)
{
    pid_t pid        = 0;

    int fd = create_ipc_fifo();
    RETURN_ON_TRUE(fd == -1, -1, destruct_ipc_fifo(););

    pid = fork();
    RETURN_ON_TRUE(pid == -1, -1,
        perror("fork error\n"););

    if (pid)
        return interface_process(fd, pid);
    else
        return functional_process(pid_to_monitor, fd);
    
    
    return 0;
}

