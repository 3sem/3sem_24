#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <time.h>

typedef struct{
    double a;             
    double b;             
    double max_f;        
    int points;          
    int points_under;    
    unsigned int seed;   
} ThreadData;


double func(double x);

void* monte_carlo(void* arg);

double get_time_diff(struct timespec start, struct timespec end);

int main(){
    int n_threads = 15; 
    printf("Используем %d потоков\n", n_threads);

    int total_points = 10000000; 
    double a = 0.0;             
    double b = 5.0;             
    double max_f = func(b);       

    pthread_t threads[n_threads];
    ThreadData thread_data[n_threads];
    int points_per_thread = total_points / n_threads;

    srand(time(NULL));

    struct timespec start_time, end_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    for (int i = 0; i < n_threads; i++){
        thread_data[i].a = a;
        thread_data[i].b = b;
        thread_data[i].max_f = max_f;
        thread_data[i].points = points_per_thread;
        thread_data[i].points_under = 0;
        thread_data[i].seed = rand() + i; 

        if (pthread_create(&threads[i], NULL, monte_carlo, &thread_data[i]) != 0) {
            perror("Error creating thread");
            exit(1);
        }
    }

    int total_points_under = 0;
    for (int i = 0; i < n_threads; i++){
        if (pthread_join(threads[i], NULL) != 0) {
            perror("Error joining thread");
            exit(1);
        }
        total_points_under += thread_data[i].points_under; 
    }

    clock_gettime(CLOCK_MONOTONIC, &end_time);

    double rectangle_area = (b - a) * max_f; 
    double integral = rectangle_area * ((double)total_points_under / total_points);

    double elapsed_time = get_time_diff(start_time, end_time);

    printf("Результат интегрирования: %f\n", integral);
    printf("Время выполнения: %f секунд\n", elapsed_time);

    return 0;
}


double func(double x){
    return x * x; 
}

void* monte_carlo(void* arg){
    ThreadData* data = (ThreadData*)arg;
    double a = data->a;
    double b = data->b;
    double max_f = data->max_f;
    int points = data->points;
    int count_under = 0;
    unsigned int seed = data->seed; 

   
    for (int i = 0; i < points; i++){
        double x = a + ((double)rand_r(&seed) / RAND_MAX) * (b - a); 
        double y = ((double)rand_r(&seed) / RAND_MAX) * max_f;       
        if (y <= func(x)){
            count_under++; 
        }
    }

    
    data->points_under = count_under;
    return NULL;
}


double get_time_diff(struct timespec start, struct timespec end){
    return (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
}