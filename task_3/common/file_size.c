#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "common_func.h"

off_t get_file_size(int fd)
{
    off_t file_size = lseek(fd, 0, SEEK_END);
    RETURN_ERROR_ON_TRUE(file_size == -1, -1,
        perror("file size detection error"););
    lseek(fd, 0, SEEK_SET);

    return file_size;
}