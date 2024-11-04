#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>

#define SHM_NAME "/monte_carlo_shm"

int main(int argc, char* argv[]) 
{
    if (argc != 2)
	{
        fprintf(stderr, "Usage: %s <number_of_threads>\n", argv[0]);
        return 1;
    }

    int n_threads = atoi(argv[1]);

    int shm_fd = shm_open(SHM_NAME, O_RDONLY, 0666);
    if (shm_fd == -1)
	{
        perror("shm_open failed");
        return 1;
    }

    double* shared_mem = mmap(NULL, n_threads * sizeof(double),
                              PROT_READ, MAP_SHARED, shm_fd, 0);
    if (shared_mem == MAP_FAILED)
	{
        perror("mmap failed");
        close(shm_fd);
        return 1;
    }

    double result = 0.0;
    for (int i = 0; i < n_threads; i++)
	{
        result += shared_mem[i];
    }

    result /= n_threads;

    printf("Integral result: %f\n", result);

    munmap(shared_mem, n_threads * sizeof(double));
    close(shm_fd);
    shm_unlink(SHM_NAME);  

    return 0;
}

