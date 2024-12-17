#include <signal.h>

#include "error.h"
#include "signal_hdl.h"


int ignore_signals( void)
{
    sigset_t mask = { 0 };
    
    handle( sigfillset( &mask));
    handle( sigprocmask( SIG_SETMASK, &mask, NULL));
    
    return 0;
}
