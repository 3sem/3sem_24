#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <stddef.h>

#define BUFFER_SIZE  8192    // 8 KB 

struct _Buffer
{
    char* buffer;
    size_t capacity;
    size_t size;

};

typedef struct _Buffer Buffer;

Buffer* create_buffer (size_t capacity);

int destroy_buffer (Buffer* buffer);

int write_to_file (Buffer* buffer, int fileDesc);

int read_from_file (Buffer* buffer, int fileDesc);

#endif // FILE_UTILS_H

