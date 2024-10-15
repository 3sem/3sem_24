#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <mbedtls/md5.h>

#include "config.h"
#include "utils.h"

#define MD5_DIGEST_LENGTH 16

int compute_md5(const char *filepath, unsigned char output[MD5_DIGEST_LENGTH]);
int compare_files_md5(const char *file1, const char *file2);

int compute_md5(const char *filepath, unsigned char output[MD5_DIGEST_LENGTH]) {
    mbedtls_md5_context ctx;
    unsigned char buffer[1024];
    size_t bytes_read;
    FILE *file = fopen(filepath, "rb");

    if (!file) {
        perror("Failed to open file");
        return -1;
    }

    mbedtls_md5_init(&ctx);
    mbedtls_md5_starts(&ctx);

    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        mbedtls_md5_update(&ctx, buffer, bytes_read);
    }

    mbedtls_md5_finish(&ctx, output);
    mbedtls_md5_free(&ctx);
    fclose(file);
    return 0;
}

int compare_files_md5(const char *file1, const char *file2) {
    unsigned char md5_1[MD5_DIGEST_LENGTH];
    unsigned char md5_2[MD5_DIGEST_LENGTH];

    if (compute_md5(file1, md5_1) != 0) {
        return -1; // Error computing md5 for file1
    }

    if (compute_md5(file2, md5_2) != 0) {
        return -1; // Error computing md5 for file2
    }

    return memcmp(md5_1, md5_2, MD5_DIGEST_LENGTH) == 0 ? 1 : 0; // 1 if equal, 0 if not equal
}

void computeTimeExchangeData(int(*exchangeData)(int, int)) {
    struct timespec start, end;
    double elapsed_time = 0;

    for (int i = 0; i < NUM_ITERATIONS; ++i) {
        int fd_read = open(TEST_FILE_INPUT, O_RDONLY);
        if (fd_read == -1) {
            perror("Error open failed");
            exit(EXIT_FAILURE);
        }
        int fd_write = open(TEST_FILE_OUTPUT, O_WRONLY | O_CREAT, 0644);
        if (fd_write == -1) {
            perror("Error open failed");
            exit(EXIT_FAILURE);
        }

        clock_gettime(CLOCK_MONOTONIC, &start);
        int result = exchangeData(fd_read, fd_write);
        clock_gettime(CLOCK_MONOTONIC, &end);

        elapsed_time += (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);
        
        close(fd_read);
        close(fd_write);

        if (compare_files_md5(TEST_FILE_INPUT, TEST_FILE_OUTPUT) == 0) {
            printf("Error exchange data\n");
            exit(EXIT_FAILURE);            
        }
    }

    printf("Time exchange data: %.2f мс\n", elapsed_time / NUM_ITERATIONS / 1e6);
}