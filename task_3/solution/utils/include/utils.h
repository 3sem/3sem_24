#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

#define SMALL_BUFFER_SIZE  8192
#define MEDIUM_BUFFER_SIZE 65536
#define LARGE_BUFFER_SIZE  1048576

typedef struct
{
    char*  data;
    size_t capacity;
    ssize_t size;
} Buffer;

int alloc_buf(Buffer* buf, size_t capacity);
int dealloc_buf(Buffer* buf);
ssize_t read_from(Buffer* buf, int file_d);
ssize_t write_to(const Buffer* buf, int file_d);

int open_read_file(const char* filename);
int open_write_file(const char* filename);

#endif // UTILS_H
