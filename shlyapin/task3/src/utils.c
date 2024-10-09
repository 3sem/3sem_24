#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <openssl/md5.h>

#include "config.h"
#include "utils.h"

#define MD5_BUFFER_SIZE 1024

int compare_files_md5(const char *file1, const char *file2);
void calculate_md5(const char *filename, unsigned char *md5_hash);

void calculate_md5(const char *filename, unsigned char *md5_hash) {
    MD5_CTX md5_ctx;
    MD5_Init(&md5_ctx);

    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    unsigned char buffer[MD5_BUFFER_SIZE];
    size_t bytes_read;

    while ((bytes_read = fread(buffer, 1, MD5_BUFFER_SIZE, file)) != 0) {
        MD5_Update(&md5_ctx, buffer, bytes_read);
    }

    MD5_Final(md5_hash, &md5_ctx);
    fclose(file);
}

// Функция для сравнения MD5-хэшей двух файлов
int compare_files_md5(const char *file1, const char *file2) {
    unsigned char md5_hash1[MD5_DIGEST_LENGTH];
    unsigned char md5_hash2[MD5_DIGEST_LENGTH];

    calculate_md5(file1, md5_hash1);
    calculate_md5(file2, md5_hash2);

    return memcmp(md5_hash1, md5_hash2, MD5_DIGEST_LENGTH) == 0;
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