//-----------------------------------------------------------------
//
// Task 3 (file sharing speed comparison) main file. 
//
//-----------------------------------------------------------------
//
// The main function accepts the file name as input.
//
// Additionally, it writes the results to the stdout.
// 
//-----------------------------------------------------------------
//
// Usage example:
// ./build/filesharing 4294967296 fourgb
// 
//-----------------------------------------------------------------

#include <time.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include "include/fifo.h"
#include "include/message-queues.h"
#include "include/shared-memory.h"
#include "include/define-check-condition-ret.h"

static const size_t NUMBER_OF_REPETITIONS = 10;

static double measure_execution_time(int func(int, size_t)
                                        , const char *file_name, size_t file_size) {
    int fd = open(file_name, O_RDONLY);
    CHECK_CONDITION_PERROR_RET(fd == -1, "open", 0.0)

    struct timespec start, stop;

    clock_gettime(CLOCK_MONOTONIC, &start);
    int res = func(fd, file_size);
    clock_gettime(CLOCK_MONOTONIC, &stop);

    CHECK_CONDITION_PRINT(res != 0)

    close(fd);

    return (stop.tv_sec - start.tv_sec) + (stop.tv_nsec - start.tv_nsec) / (double) 1000000000;
}

static double measure_average_time(int func(int, size_t)
                                        , const char *file_name, size_t file_size) {
    double sum = 0.0;

    for (int i = 0; i < NUMBER_OF_REPETITIONS; ++i) {
        sum += measure_execution_time(func, file_name, file_size);
    }

    return sum / NUMBER_OF_REPETITIONS;
}

static size_t get_file_size(const char *file_name) {
    FILE *file = fopen(file_name, "r");
    CHECK_CONDITION_PERROR_RET(file == NULL, "fopen", 0)

    fseek(file, 0L, SEEK_END);
    size_t file_size = ftell(file);

    fclose(file);

    return file_size;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "argc < ! Look at the instructions for use!\n");
        return -1;
    }

    size_t file_size = get_file_size(argv[1]);

    double fifo_res = measure_average_time(fifo_translate_file, argv[1], file_size);
    double msgq_res = measure_average_time(msgq_translate_file, argv[1], file_size);
    // double smem_res = measure_average_time(smem_translate_file, argv[1], file_size);

    printf("fifo_res: %lf\n", fifo_res);
    printf("msgq_res: %lf\n", msgq_res);
    // printf("smem_res: %lf\n", smem_res);

    return 0;
}
