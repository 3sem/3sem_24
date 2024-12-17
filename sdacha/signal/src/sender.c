#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/mman.h>
#include <sys/shm.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

#include "error.h"
#include "shmem.h"
#include "sender.h"


static int
wait_receiver( void)
{
    int signal = 0; 
    sigset_t mask = { 0 };
    
    handle( sigemptyset( &mask));
    handle( sigaddset  ( &mask, SIGUSR1));
    
    if ( sigwait( &mask, &signal) > 0 )
    {
        perror( "sigwait");
        return -1;
    }
    
    return 0;
}

static int
wait_prepared( void)
{
    int signal = 0; 
    sigset_t mask = { 0 };
    
    handle( sigemptyset( &mask));
    handle( sigaddset  ( &mask, SIGUSR2));
    
    if ( sigwait( &mask, &signal) > 0 )
    {
        perror( "sigwait");
        return -1;
    }
    
    return 0;
}

static int
signal_receiver( const pid_t pid,
                 const int rel_addr)
{
    union sigval sv = { 0 };
    sv.sival_int = rel_addr;
    
    handle( sigqueue( pid, SIGUSR1, sv));
    
    return 0;
}

static int
get_rel_addr( const int n_pages)
{
    return ((double)rand() * (n_pages - 1) / RAND_MAX) * getpagesize()
          + (double)rand() * getpagesize() / RAND_MAX;
}

static int
write_shm( char *const shm)
{
    const char msg[] = "Some useful message.\n";
    
    strcpy( shm, msg);
    
    return 0;
}

static int write_log( FILE *const log, 
                      const int counter,
                      const int rel_addr)
{
    const int nbytes = fprintf( log, "%d: %d\n"
                                     "------\n",
                                     counter, rel_addr);

    if ( nbytes < 0 )
    {
        perror( "fprintf");
        return -1;
    }
    
    return 0;
}


int sender( const pid_t pid,
            const char *const log_path, 
            const int n_pages,
            const int times)
{
    handle( wait_prepared());
    
    char *const shm = (char *)get_shm( n_pages);
    handlep( shm);
    
    FILE *const log = fopen( log_path, "wb");
    handlen( log);
    
    for ( int i = 0; i < times; i++ )
    {
        const int rel_addr = get_rel_addr( n_pages);
        
        write_shm( shm + rel_addr);
        
        handle( write_log( log, rel_addr, i),
                fclose( log));
        handle( signal_receiver( pid, rel_addr),
                fclose( log));
        handle( wait_receiver(),
                fclose( log));
    }

    handle( fclose( log));
    
    return 0;
}
