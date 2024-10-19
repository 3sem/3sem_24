#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#include "IO.h"

int buffer_ctor(Buffer* buffer, size_t capacity) {
    assert(buffer);

    buffer->buffer = (char*) calloc(capacity, sizeof(char));
    if (!(buffer->buffer)) {
        perror("calloc error\n");
        return 1;
    }

    buffer->capacity = capacity;
    buffer->size     = 0;

    return 0;
}

int buffer_dtor(Buffer* buffer) {
    assert(buffer);

    free(buffer->buffer);
    buffer->capacity = 0;
    buffer->size     = 0;

    return 0;
}

int read_from_file(Buffer* buffer, int file_d) {
    assert(buffer);

    buffer->size = read(file_d, buffer->buffer, buffer->capacity);

    if (buffer->size == -1) {
        perror("read error\n");
        return -1;
    }

    return buffer->size;
}

int write_to_file(const Buffer* buffer, int file_d) {
    assert(buffer);

    int written_bytes = write(file_d, buffer->buffer, buffer->size);

    if (written_bytes == -1) {
        perror("write error\n");
        return -1;
    }

    return written_bytes;
}

int crate_output_file(const char* filename) {
    assert(filename);

    int file_d = open(filename, O_CREAT | O_WRONLY, S_IRWXU);
    if (file_d == -1) {
        perror("open output error\n");
    }

    return file_d;
}

int open_input_file(const char* filename) {
    assert(filename);

    int file_d = open(filename, O_RDONLY);
    if (file_d == -1) {
        perror("open input error\n");
    }

    return file_d;
}


