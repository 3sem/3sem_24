#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

#define A 0.0  // Начало интервала интегрирования
#define B 1.0  // Конец интервала интегрирования
#define SHM_NAME "/monte_carlo_shm"

typedef struct
	{
    int thread_id;
    int points;
    double* shared_mem;
    double segment_start;  
    double segment_end;   
} ThreadData;

double function(double x)
	{
    return x * x;
}

void* monte_carlo_thread(void* arg)
	{
    ThreadData* data = (ThreadData*) arg;
    double sum = 0.0;
    unsigned int seed = data->thread_id;

    for (int i = 0; i < data->points; i++)
	{
        double x = data->segment_start + (data->segment_end - data->segment_start) * rand_r(&seed) / (double)RAND_MAX;
        double y = function(x);
        sum += y;
    }

    data->shared_mem[data->thread_id] = sum / data->points * (data->segment_end - data->segment_start);
    pthread_exit(NULL);
}

int main(int argc, char* argv[])
	{
    if (argc != 3)
	{
        fprintf(stderr, "Usage: %s <number_of_threads> <total_points>\n", argv[0]);
        return 1;
    }

    int n_threads = atoi(argv[1]);
    int N = atoi(argv[2]);
    int points_per_thread = N / n_threads;
    int remainder_points = N % n_threads;

    // Calculate the width of each segment
    double segment_width = (B - A) / n_threads;

    // Set up shared memory
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1)
	{
        perror("shm_open failed");
        return 1;
    }

    if (ftruncate(shm_fd, n_threads * sizeof(double)) == -1)
	{
        perror("ftruncate failed");
        return 1;
    }

    double* shared_mem = mmap(NULL, n_threads * sizeof(double), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared_mem == MAP_FAILED)
	{
        perror("mmap failed");
        return 1;
    }

    pthread_t threads[n_threads];
    ThreadData thread_data[n_threads];

    for (int i = 0; i < n_threads; i++)
	{
        thread_data[i].thread_id = i;
        thread_data[i].points = points_per_thread + (i < remainder_points ? 1 : 0);
        
        // Calculate the segment for this thread
        thread_data[i].segment_start = A + i * segment_width;
        thread_data[i].segment_end = A + (i + 1) * segment_width;
        
        thread_data[i].shared_mem = shared_mem;
        pthread_create(&threads[i], NULL, monte_carlo_thread, &thread_data[i]);
    }

    for (int i = 0; i < n_threads; i++)
	{
        pthread_join(threads[i], NULL);
    }

    double total_integral = 0.0;
    for (int i = 0; i < n_threads; i++)
	{
        total_integral += shared_mem[i];
    }

    munmap(shared_mem, n_threads * sizeof(double));
    close(shm_fd);

    return 0;
}

