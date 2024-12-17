#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>

#include "error.h"
#include "shmem.h"
#include "signal_hdl.h"
#include "sender.h"
#include "receiver.h"


int main( const int argc, const char *argv[])
{
    if ( argc != 4 )
    {
        return 1;
    }
    
    const int n_pages = 8;
    const int times   = atoi( argv[1]);
    const char *const   sender_log_path = argv[2];
    const char *const receiver_log_path = argv[3];
    
    handle( ignore_signals());
    handle( create_shm( n_pages));
    
    const pid_t pid = fork();
    handle( pid,
            free_shm( n_pages));
    
    if ( pid == 0 )
    {
        handle( receiver( receiver_log_path,
                          n_pages, times));
    }
    else
    {
        handle( sender( pid, sender_log_path,
                        n_pages, times),
                free_shm( n_pages));
        
        handle( free_shm( n_pages));
    }
    
    return 0;
}
