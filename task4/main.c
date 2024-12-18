#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define NUM_THREADS 100
#define N 1e8 

typedef struct {
    int thread_id;
    int num_samples;
    double result;
} thread_data_t;

double f(double x) {
    return x * x * x;
}

void* monte_carlo(void* arg) {
    thread_data_t* data = (thread_data_t*)arg;
    double local_sum = 0.0;

    unsigned int seed;
    srand(time(NULL));

    for (int i = 0; i < data->num_samples; i++) {
        double x = ((double)rand_r(&seed) / RAND_MAX) * (4.0 - 0.0) + 0.0; 
        local_sum += f(x);
    }
    
    data->result = local_sum;
    pthread_exit(NULL);
}

int main() {
    pthread_t threads[NUM_THREADS];
    thread_data_t thread_data[NUM_THREADS];
    
    int samples_per_thread = N / NUM_THREADS;

    struct timespec start, stop;

    //double start = clock();
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_data[i].thread_id = i;
        thread_data[i].num_samples = samples_per_thread;
        pthread_create(&threads[i], NULL, monte_carlo, (void*)&thread_data[i]);
    }
    
    double total_sum = 0.0;
    
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
        total_sum += thread_data[i].result;
    }
    
    double integral = (4.0 - 0.0) * total_sum / N;

    clock_gettime(CLOCK_MONOTONIC, &stop);

    double exec_time = (stop.tv_sec - start.tv_sec) + (stop.tv_nsec - start.tv_nsec) / (double) 1e9;

    printf("Приближенное значение интеграла: %f\n", integral);
    printf("Количество потоков: %d\n", NUM_THREADS);
    printf("Время работы: %f мс\n", 1000*exec_time);
    
    return 0;
}