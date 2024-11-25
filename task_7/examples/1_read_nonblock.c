#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main() {
    int flags;

    // Get the current flags for the stdin file descriptor
    flags = fcntl(0, F_GETFL);
    if (flags == -1) {
        perror("fcntl");
        return 1;
    }

    // Set the O_NONBLOCK flag
    flags |= O_NONBLOCK;
    if (fcntl(0, F_SETFL, flags) == -1) {
        perror("fcntl");
        return 1;
    }

    // Now reads on stdin will be non-blocking
    char buffer[1024];
    int nread = read(0, buffer, 1024);
    if (nread == -1) {
        if (errno == EAGAIN) {
            printf("(stdin empty)\n");
        } else {
            perror("read");
            return 1;
        }
    } else if (nread == 0) {
        printf("End of input\n");
    } else {
        buffer[nread] = '\0';
        printf("Read: %s\n", buffer);
    }

    return 0;
}
