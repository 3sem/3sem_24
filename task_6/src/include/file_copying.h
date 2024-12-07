#ifndef COPY_FILE_TO_FILE
#define COPY_FILE_TO_FILE

#include "sys/types.h"

int file_to_file(const int to_fd, const int from_fd);

off_t get_file_size(int fd);

#endif