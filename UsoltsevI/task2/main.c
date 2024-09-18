#include <stdio.h>
#include <unistd.h>
#include "bigpipe.h"
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>


int main() {
    printf("BigPipe started\n");

    clock_t start = clock();

    Pipe* mpipe = construct_pipe(1024);

    pid_t pid = fork();

    size_t sent = 0;
    size_t recieved = 0;

    if (pid < 0) { // error
        perror("fork failure");

    } else if (pid > 0) { // parent way
        sent = pipe_send_file(mpipe, pid, STDIN_FILENO);
        recieved = pipe_recieve_file(mpipe, pid, STDOUT_FILENO);

    } else { // child way
        int fd = open("temporary", O_RDWR | O_APPEND | O_CREAT, O_CLOEXEC);

        printf("fd = %d\n", fd);

        pipe_recieve_file(mpipe, pid, fd);
        pipe_send_file(mpipe, pid, fd);

        close(fd);

        return 0;
    }

    delete_pipe(mpipe);

    clock_t end = clock();

    printf("BigPipe finished\n");
    printf("Bytes sent: %lu, bytes recieved: %lu\n", sent, recieved);
    printf("Executed time: %lf\n", ((double)(end - start)) / CLOCKS_PER_SEC);

    return 0;
}
