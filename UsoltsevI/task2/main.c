//-----------------------------------------------------------------
//
// bigpipe main file
//
//-----------------------------------------------------------------

#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include "bigpipe.h"

static const char* TEMPORARY_FILE_NAME = "temporary";
static const char* OUT_FILE_NAME       = "out";

static size_t str_to_size_t(const char *str) {
    int i = 0;
    size_t result = 0;

    while (str[i] != '\0') {
        ++i;
    }

    --i;
    size_t mul = 1;

    while (i >= 0 && str[i] <= '9' && str[i] >= '0') {
        result += (str[i] - '0') * mul;
        --i;
        mul *= 10;
    }

    return result;
}

int main(int argc, char *argv[]) {
    printf("--BigPipe sending started\n");

    clock_t start = clock();

    Pipe* mpipe = construct_pipe(1024);

    if (mpipe == NULL) {
        fprintf(stderr, "mpipe == NULL\n");
        return 1;
    }

    size_t file_size = 0;

    if (argc > 1) {
        file_size  = str_to_size_t(argv[1]);
        printf("----------------file_size: %ld\n", file_size);
    }

    pipe_set_data_size(mpipe, file_size);

    size_t sent     = 0;
    size_t recieved = 0;

    pid_t pid = fork();

    pipe_set_pid(mpipe, pid); // for both of parent and child

    if (pid < 0) { // error
        perror("fork failure");

    } else if (pid > 0) { // parent way
        int fd   = open(OUT_FILE_NAME, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
        sent     = pipe_send_file   (mpipe, STDIN_FILENO);
        recieved = pipe_recieve_file(mpipe, fd);
        close(fd);
        remove(OUT_FILE_NAME);

    } else { // child way
        int fd = open(TEMPORARY_FILE_NAME, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);

        size_t child_recieved = pipe_recieve_file(mpipe, fd);
        printf("----Recieved by the child: %ld\n", child_recieved);

        close(fd);
        fd = open("temporary", O_RDONLY);

        size_t child_sent = pipe_send_file(mpipe, fd);
        printf("--------Sent by the child: %ld\n", child_sent);

        close(fd);

        remove(TEMPORARY_FILE_NAME);

        return 0;
    }

    delete_pipe(mpipe);

    clock_t end = clock();

    printf("-BigPipe sending finished\n");
    printf("---------------\033[1;33mBytes sent: \033[1;35m%lu\033[0m\n", sent); 
    printf("-----------\033[1;33mBytes recieved: \033[1;35m%lu\033[0m\n", recieved);
    printf("------------\033[1;33mExecuted time: \033[1;35m%lf sec\033[0m\n", ((double)(end - start)) / CLOCKS_PER_SEC);

    return 0;
}
