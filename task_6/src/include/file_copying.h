#ifndef COPY_FILE_TO_FILE
#define COPY_FILE_TO_FILE

#include "sys/types.h"

#define MAP_BUFF_SIZE 256

int file_to_file(const int to_fd, const int from_fd);

int maps_to_file(const int fd, const char *map_path);

off_t get_file_size(int fd);

#endif