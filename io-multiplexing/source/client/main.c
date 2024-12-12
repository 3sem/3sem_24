#include "../common/file_utils.h"
#include "../common/log_utils.h"

#include <fcntl.h>
#include <linux/limits.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char* argv[])
{
    const char* register_fifo = "register";

    int register_fd = open(register_fifo, O_WRONLY | O_NONBLOCK);

    Buffer* buffer = create_buffer(2 * PATH_MAX);

    fgets(buffer->buffer, buffer->capacity, stdin);

    buffer->size = strlen(buffer->buffer);

    LOG("[buffer]: %s, size: %zu\n", buffer->buffer, buffer->size);

    write_to_file(buffer, register_fd);

    destroy_buffer(buffer);

    return 0;
}
