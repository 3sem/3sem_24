#include <stdio.h>
//#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <limits.h>
#include <fcntl.h>
#include "debugging.h"
#include "open_pid_maps.h"

int open_maps(const char *path)
{
    int map_fd = open(path, O_RDONLY);
    RETURN_ON_TRUE(map_fd == -1, -1, perror("map file opening error\n"););

    return map_fd;
}