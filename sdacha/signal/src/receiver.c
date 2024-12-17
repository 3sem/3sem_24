#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <sys/mman.h>
#include <sys/shm.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

#include "error.h"
#include "shmem.h"
#include "receiver.h"


static volatile int  counter    = 0;
static volatile bool is_working = true;

static char *shm = NULL;
static FILE *log = NULL;


static char*
read_shm( const char *const addr)
{
    static char buffer[4096] = "";
    
    memset( buffer, 0, sizeof( buffer));
    strcpy( buffer, addr);
    
    return buffer;
}

static int
write_log( const int rel_addr,
           const char *const buffer)
{
    const int nbytes = fprintf( log, "%d: %d: %s"
                                     "------\n",
                                     counter,
                                     rel_addr,
                                     buffer);
    
    if ( nbytes < 0 )
    {
        perror( "fprintf");
        is_working = false;
        
        return -1;
    }
    
    return 0;
}

static int
signal_sender( void)
{
    union sigval sv = { 0 };
    
    handle( sigqueue( getppid(), SIGUSR1, sv));
    
    return 0;
}

static int
signal_prepared( void)
{
    union sigval sv = { 0 };
    
    handle( sigqueue( getppid(), SIGUSR2, sv));
    
    return 0;
}

static void
handle_signal( int sig,
               siginfo_t* si,
               void* ucontext)
{
    const int rel_addr = si->si_value.sival_int;
    const char *const buffer = read_shm( shm + rel_addr);
    
    if ( write_log( rel_addr, buffer) == -1 )
    {
        is_working = false;
    }
    counter++;
    
    if ( signal_sender() == -1 )
    {
        is_working = false;
    }
}

static int
init_handler( void)
{
    struct sigaction sa = { 0 };
    
    handle( sigemptyset( &sa.sa_mask));
    sa.sa_sigaction = handle_signal;
    sa.sa_flags     = SA_SIGINFO;
    
    handle( sigaction( SIGUSR1, &sa, NULL));
    
    return 0;
}

static int
init_signals( void)
{
    sigset_t mask = { 0 };
    
    handle( sigemptyset( &mask));
    handle( sigaddset  ( &mask, SIGUSR1));
    handle( sigprocmask( SIG_UNBLOCK, &mask, NULL));
    
    return 0;
}


int
receiver( const char *const log_path, 
          const int n_pages, const int times)
{
    shm = (char *)get_shm( n_pages);
    handlep( shm);
    
    handle( init_signals());
    handle( init_handler());
    
    log = fopen( log_path, "wb");
    handlen( log);
    
    handle( signal_prepared(),
            fclose( log));
    
    while ( counter != times && is_working )
    { /* nothing */ }
    
    handle( fclose( log));
    
    return 0;
}
