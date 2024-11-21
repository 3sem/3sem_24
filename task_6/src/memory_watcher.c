#include <stdio.h>
#include <unistd.h>
#include "memory_watcher.h"

int memory_manage()
{
    while (1)
    {
        sleep(2);
        printf("action\n");
    }

    return 0;
}