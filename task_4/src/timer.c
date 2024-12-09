#include <sys/time.h>
#include <stdio.h>
#include "timer.h"

static struct timeval tv1, tv2, dtv;
static int hold = 0;

void start_timer()
{   
    gettimeofday(&tv1, NULL); 

    hold = 1;
}

long long int stop_timer()
{ 
    gettimeofday(&tv2, NULL);

    dtv.tv_sec  = tv2.tv_sec  - tv1.tv_sec;
    dtv.tv_usec = tv2.tv_usec - tv1.tv_usec;

    if (dtv.tv_usec < 0) 
    {
        dtv.tv_sec--; 
        dtv.tv_usec += 1000000; 
    }

    return dtv.tv_sec * 1000 + dtv.tv_usec / 1000;
}

