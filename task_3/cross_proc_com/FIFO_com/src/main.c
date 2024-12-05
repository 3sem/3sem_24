#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include <time.h>

#include "fifo_com.h"
#include "IO.h"

int main(int argc, char* arg[]) {
    unlink(FIFO_NAME);

    if(mkfifo(FIFO_NAME, 0666)) {
        perror("mkfifo error\n");
        return 1;
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("Fork error\n");
        return 1;
    }

    // Parent process (writer)
    if (pid) {
        Buffer buffer = {};
        buffer_ctor(&buffer, BUFFER_CAPACITY);

        int write_d = open(FIFO_NAME, O_WRONLY);    // open FIFO
        if (write_d == -1) {
            perror("fifo open error");
            return -1;
        }

        int input_d = open_input_file(INPUT_FILENAME);
        if (input_d == -1) return 1;

        while (read_from_file(&buffer, input_d)) {
            write_to_file(&buffer, write_d);
        }

        buffer_dtor(&buffer);
        close(write_d);
        close(input_d);
    }
    // Child process (accepter)
    else {
        Buffer buffer = {};
        buffer_ctor(&buffer, BUFFER_CAPACITY);

        int read_d   = open(FIFO_NAME, O_RDONLY);
        if (read_d == -1) {
            perror("open fifo");
            return -1;
        }

        int output_d = crate_output_file(OUTPUT_FILENAME);
        if (output_d == -1) return 1;

        clock_t proc_time = clock();

        while (read_from_file(&buffer, read_d)) {
            write_to_file(&buffer, output_d);
        } 

        proc_time = clock() - proc_time;
        printf("Receive time: %ld\n", proc_time);

        buffer_dtor(&buffer);
        close(read_d);
        close(output_d);
    }
    
    return 0;
}