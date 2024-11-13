#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <assert.h>
#include <sys/stat.h>
#include "process_results.h"
#include "send_results.h"
#include "integral_calc.h"

int open_shm(long int **arr_ptr, const size_t shm_size);

void shm_destr(long int *shm_arr, const size_t shm_size, const int shm_fd);


int print_result()
{
    long int *res_arr   = NULL;
    long int res_sum    = 0;
    double   result     = 0;   

    int shm_fd = open_shm(&res_arr, NUMBER_OF_THREADS * sizeof(long int));
    RETURN_ERROR_ON_TRUE(shm_fd == -1, -1);

    for (int i = 0; i < NUMBER_OF_THREADS; i++)
        res_sum += res_arr[i];
    
    result = ((double)res_sum / POINTS_NUMBER) * X_LIMIT * Y_LIMIT;

    printf("\033[0;33m> Function integral equals %lf\033[0m\n", result);

    shm_destr(res_arr, NUMBER_OF_THREADS * sizeof(long int), shm_fd);

    return 0;
}

int open_shm(long int **arr_ptr, const size_t shm_size)
{
    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    RETURN_ERROR_ON_TRUE(shm_fd == -1, -1,
        perror("shared mem open error"););

    int err_num = ftruncate(shm_fd, (off_t)shm_size);
    RETURN_ERROR_ON_TRUE(err_num == -1, -1,
        perror("ftruncate call error occured"););

    void *addr = mmap(NULL, shm_size, PROT_WRITE | PROT_READ, MAP_SHARED, shm_fd, 0);
    RETURN_ERROR_ON_TRUE(addr == MAP_FAILED, -1, 
        perror("mapping error"););

    *arr_ptr = (long int *)addr; 

    return shm_fd;
}

void shm_destr(long int *shm_arr, const size_t shm_size, const int shm_fd)
{
    assert(shm_arr);

    if (munmap(shm_arr, shm_size))
        perror("unmapping error occured");
    
    close(shm_fd);

    shm_unlink(SHM_NAME);

    return;
}