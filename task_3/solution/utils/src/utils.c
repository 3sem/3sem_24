#include "utils.h"

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

int alloc_buf(Buffer* buffer, size_t capacity)
{
    buffer->data = (char*) calloc(capacity, sizeof(char));
    if (!(buffer->data))
	{
        perror("calloc error\n");
        return 1;
    }

    buffer->capacity = capacity;
    buffer->size     = 0;

    return 0;
}

int dealloc_buf(Buffer* buffer)
{
    free(buffer->data);
    buffer->capacity = 0;
    buffer->size     = 0;

    return 0;
}

ssize_t read_from(Buffer* buffer, int file_d)
{
    buffer->size = read(file_d, buffer->data, buffer->capacity);

    if (buffer->size == -1)
	{
        perror("read error\n");
        return -1;
    }

    return buffer->size;
}

ssize_t write_to(const Buffer* buffer, int file_d)
{
    ssize_t written_bytes = write(file_d, buffer->data, (size_t)buffer->size);

    if (written_bytes == -1)
	{
        perror("write error\n");
        return -1;
    }

    return written_bytes;
}

int open_write_file(const char* filename)
{
    int file_d = open(filename, O_CREAT | O_WRONLY, S_IRWXU);
    if (file_d == -1)
	{
        perror("open output error\n");
    }

    return file_d;
}

int open_read_file(const char* filename)
{
    int file_d = open(filename, O_RDONLY);
    if (file_d == -1)
	{
        perror("open input error\n");
    }

    return file_d;
}
