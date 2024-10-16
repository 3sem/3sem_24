#ifndef IO_H
#define IO_H

#include <stdio.h>

#define SMALL_BUFFER_SIZE  8192
#define MEDIUM_BUFFER_SIZE 65536
#define LARGE_BUGGER_SIZE  1048576

typedef struct Buffer_ {
    char* buffer;
    size_t capacity;
    size_t size;
} Buffer;

int buffer_ctor(Buffer* buffer, size_t capacity);
int buffer_dtor(Buffer* buffer);
int read_from_file(Buffer* buffer, int file_d);
int write_to_file(const Buffer* buffer, int file_d);

int open_input_file(const char* filename);
int crate_output_file(const char* filename);

#endif