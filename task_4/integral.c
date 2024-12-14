#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

#define SHM_NAME "/integral_shm"

typedef struct {
    double start;
    double end;
    double n_point;
    double * shared_mem;
    int thread_idx;
} Integral;

static double function(double x) {
    return x * x * x;
}
unsigned int get_seed(int thread_idx) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec ^ ts.tv_nsec) + thread_idx;
}

void* monte_carlo(void* arg) {
    Integral *integral = (Integral *)arg;
    double count_inside = 0;
    unsigned int seed = get_seed(integral->thread_idx);
  
    double x_random, y_random, y_max = function(integral->end);
    for (int i = 0; i < integral->n_point; i++) {
        x_random = ((double)rand_r(&seed) / RAND_MAX) * (integral->end - integral->start) + integral->start;
        y_random = ((double)rand_r(&seed) / RAND_MAX) * y_max;

        if (y_random <= function(x_random) && (x_random >= 0)) {
            count_inside++;
        }
    }

    double result = (count_inside / integral->n_point) *
                   (integral->end - integral->start) *
                    y_max;
    
    integral->shared_mem[integral->thread_idx] = result;
    pthread_exit(NULL);
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc < 5) {
        fprintf(stderr, "Usage: %s number_of_threads min_value max_value number_of_points\n", argv[0]);
        return 1;
    }

    int CountThreads = atoi(argv[1]);
    double Start = atof(argv[2]);
    double End = atof(argv[3]);
    int n_points = atoi(argv[4]);

    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shared memory failed");
        return 1;
    }

    size_t shm_size =  CountThreads * sizeof(double);
    if (ftruncate(shm_fd, shm_size) == -1) {
        perror("ftruncate failed");
        close(shm_fd);
        return 1;
    }

    double *shared_mem = mmap(NULL, shm_size, PROT_READ | PROT_WRITE,
                                  MAP_SHARED, shm_fd, 0);
    if (shared_mem == MAP_FAILED) {
        perror("mmap failed");
        close(shm_fd);
        return 1;
    }

    pthread_t threads[CountThreads];
    Integral integrals[CountThreads];
    double step = (End - Start) / CountThreads;
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);


    clock_t start_time = clock();

    for (int i = 0; i < CountThreads; i++) {
        integrals[i].start = Start + i * step;
        integrals[i].end = Start + (i + 1) * step;
        integrals[i].n_point = n_points/CountThreads;
        integrals[i].shared_mem = shared_mem;
        integrals[i].thread_idx = i;

        pthread_create(&threads[i], NULL, monte_carlo, &integrals[i]);
    }

    for (int i = 0; i < CountThreads; i++) {
        pthread_join(threads[i], NULL);
    }

    double total_result = 0.0;
    for(int i = 0; i < CountThreads; i++) {
        total_result += shared_mem[i];
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    double time_spent = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
   
    
    
     printf("Time: %f millisec.\n", time_spent);
    munmap(shared_mem, shm_size);
    close(shm_fd);
    return 0;
}