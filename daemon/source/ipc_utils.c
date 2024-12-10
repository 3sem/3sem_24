#include <stdlib.h>
#include <sys/syslog.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "ipc_utils.h"


void setup_ipc(const char* fifo_path) 
{
    if (mkfifo(fifo_path, 0666) == -1) {
        if (errno != EEXIST) 
        {
            syslog(LOG_ERR, "fifo already exists");
            exit(EXIT_FAILURE);
        }
    }
}

void cleanup_ipc(const char* fifo_path) 
{
    unlink(fifo_path);
}

