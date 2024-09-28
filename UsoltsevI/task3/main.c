//-----------------------------------------------------------------
//
// Task 3 (file sharing speed comparison) main file. 
//
//-----------------------------------------------------------------
//
// The main function accepts the file size as input.
// The file is read from stdin. Use stream redirection
// to send another file to the program.
//
// Additionally, it writes the results to the stdout.
// 
//-----------------------------------------------------------------
//
// Usage example:
// ./build/filesharing 4294967296 < fourgb
// 
//-----------------------------------------------------------------

#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include "include/fifo.h"
#include "include/message-queues.h"
#include "include/shared-memory.h"
#include "include/str-to-size-t.h"

static const size_t NUMBER_OF_REPETITIONS = 10;

static double measure_execution_time(int func(int, size_t)
                                        , int arg1, size_t arg2) {
    struct timespec start, stop;

    clock_gettime(CLOCK_MONOTONIC, &start);
    func(arg1, arg2);
    clock_gettime(CLOCK_MONOTONIC, &stop);

    return (stop.tv_sec - start.tv_sec) + (stop.tv_nsec - start.tv_nsec) / (double) 1000000000;
}

static double measure_avarage_time(int func(int, size_t)
                                        , int arg1, size_t arg2) {
    double sum = 0.0;

    for (int i = 0; i < NUMBER_OF_REPETITIONS; ++i) {
        sum += measure_execution_time(func, arg1, arg2);
    }

    return sum / NUMBER_OF_REPETITIONS;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "argc < 2! Look at the instructions for use!\n");
        return -1;
    }

    size_t file_size = str_to_size_t(argv[0]);

    double fifo_res = measure_average_time(fifo_translate_file, STDIN_FILENO, file_size);
    double msgq_res = measure_average_time(msgq_translate_file, STDIN_FILENO, file_size);
    double smem_res = measure_average_time(smem_translate_file, STDIN_FILENO, file_size);

    printf("fifo_res: %lf\n", fifo_res);
    printf("msgq_res: %lf\n", msgq_res);
    printf("smem_res: %lf\n", smem_res);

    return 0;
}
