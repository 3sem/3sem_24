#include <stdio.h>
#include <unistd.h>
#include "bigpipe.h"
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include <fcntl.h>
#include <bits/fcntl-linux.h>
// #include <asm-generic/fcntl.h>


int main() {
    printf("BigPipe started\n");

    clock_t start = clock();

    Pipe* mpipe = construct_pipe(1024);

    if (mpipe == NULL) {
        fprintf(stderr, "mpipe == NULL\n");
        return 1;
    }

    size_t sent     = 0;
    size_t recieved = 0;

    pid_t pid = fork();

    pipe_set_pid(mpipe, pid); // for both of parent and child

    if (pid < 0) { // error
        perror("fork failure");

    } else if (pid > 0) { // parent way
        int fd = open("out", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
        sent     = pipe_send_file   (mpipe, STDIN_FILENO);
        recieved = pipe_recieve_file(mpipe, fd);
        close(fd);

    } else { // child way
        int fd = open("temporary", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);

        printf("fd = %d\n", fd);

        size_t child_recieved = pipe_recieve_file(mpipe, fd);
        printf("child_recieved: %ld\n", child_recieved);

        close(fd);

        fd = open("temporary", O_RDONLY);

        size_t child_sent = pipe_send_file(mpipe, fd);
        printf("child_sent: %ld\n", child_sent);

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
