#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "debugging.h"
#include "file_copying.h"

int file_to_file(const int to_fd, const int from_fd)
{
    LOG("-------------------file to file converting-------------------------\n");

    LOG("> file descriptors are: %d and %d\n", to_fd, from_fd);

    off_t file_size = get_file_size(from_fd);
    RETURN_ON_TRUE(file_size == -1, -1);
    RETURN_ON_TRUE(file_size == 0, 0);

    LOG("> file size is %ld\n", file_size);

    char *buff = (char *)calloc((size_t)file_size, sizeof(char));
    RETURN_ON_TRUE(!buff, -1, 
        printf(MEM_ALC_ERR_MSG);
        close(to_fd);
        close(from_fd););

    ssize_t bytes_read = read(from_fd, buff, (size_t)file_size);
    RETURN_ON_TRUE(bytes_read == -1, -1, 
        perror("file read error");
        close(to_fd);
        close(from_fd););

    LOG("> %ld bytes were read to buff\n", bytes_read);

    bytes_read = write(to_fd, buff, (size_t)file_size);
    RETURN_ON_TRUE(bytes_read == -1, -1, 
        perror("file write error");
        close(to_fd);
        close(from_fd););

    LOG("> %ld bytes were written from buff\n", bytes_read);

    free(buff);

    return 0;
}

off_t get_file_size(int fd)
{
    off_t file_size = lseek(fd, 0, SEEK_END);
    RETURN_ON_TRUE(file_size == -1, -1,
        perror("file size detection error"););
    lseek(fd, 0, SEEK_SET);

    LOG("> file size = %ld\n", file_size);

    return file_size;
}