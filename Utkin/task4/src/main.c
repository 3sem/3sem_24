#include <stdio.h>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>
#include "integral.h"

double Func(double x) {
    return 1/x + x*x;
}

int main() {
    int fd = open("data.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {perror("Ошибка открытия файла"); return 1;}
    char buffer[2048];
    

    double a = 1.0;
    double b = 1000.0;
    int processCount = 1;

    struct timespec start, end;
    double res_time = 0.0;

    int max_nstreams = 1000;
    int nstarts = 10;
    for (int i = 1; i <= max_nstreams; ++i) {
        for (int j = 0; j < nstarts; ++j) {
            clock_gettime(CLOCK_MONOTONIC, &start);
            double res = computeIntegral(a, b, Func, i);
            clock_gettime(CLOCK_MONOTONIC, &end);
            res_time += (double)(end.tv_sec - start.tv_sec) * 1e9 + (double)(end.tv_nsec - start.tv_nsec);
        }

        res_time /= nstarts;
        printf("For %3d streams time: %g мс\n", i, res_time / 1e6);
        int length = snprintf(buffer, sizeof(buffer), "%d %g\n", i, res_time / 1e6);
        write(fd, buffer, length);
        res_time = 0.0;
    }

    close(fd);

    return 0;
}
