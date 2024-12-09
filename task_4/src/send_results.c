#include <unistd.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <assert.h>
#include <fcntl.h>
#include <errno.h>
#include "send_results.h"

int result_shm_arr_create(long int **arr_ptr, const size_t shm_size)
{
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_EXCL | O_RDWR, 0666);
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

void shm_close(long int *shm_arr, const size_t shm_size, const int shm_fd)
{
    assert(shm_arr);

    if (munmap(shm_arr, shm_size))
        perror("unmapping error occured");
    
    close(shm_fd);

    return;
}