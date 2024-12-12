#include "file_utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>

Buffer* create_buffer(size_t capacity) 
{
    Buffer* buffer = (Buffer*) calloc (1, sizeof(Buffer));

    if (buffer == NULL)
    {
        perror("Unable to allocte memory for buffer");

        return NULL;
    }

    buffer->buffer = (char*) calloc (capacity, sizeof(char));

    if (buffer->buffer == NULL) 
    {
        perror("Failed to allocate buffer memory");
        free(buffer->buffer);

        return NULL;
    }

    buffer->capacity = capacity;
    buffer->size = 0;

    return buffer;
}

int destroy_buffer(Buffer* buffer)
{
    assert(buffer);

    if (buffer->buffer == NULL)
    {
        perror("Buffer's buffer is dead");

        return 1;
    }

    free(buffer->buffer);

    buffer->size     = 0;
    buffer->capacity = 0;

    return 0;
}

int write_to_file(Buffer* buffer, int fileDesc)
{
    assert(buffer);

    int nBytes = write(fileDesc, buffer->buffer, buffer->size);

    if (nBytes == -1)
    {
        perror("Unable to write");

        return -1;
    }

    return nBytes;
}

int read_from_file(Buffer* buffer, int fileDesc)
{
    assert(buffer);

    buffer->size = read(fileDesc, buffer->buffer, buffer->capacity);
    
    if (buffer->size == -1)
    {
        perror("Unable to read");

        return -1;
    }

    return buffer->size;
}
