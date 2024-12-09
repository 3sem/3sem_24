#ifndef SHARED_MEM_HEAD
#define SHARED_MEM_HEAD

#include <sys/types.h>

#define SHM_NAME            "integral_res"

#define RETURN_ERROR_ON_TRUE(statement, return_val, ...)                \
    do                                                                  \
    {                                                                   \
        if (statement)                                                  \
        {                                                               \
            __VA_ARGS__                                                 \
            return return_val;                                          \
        }                                                               \
    } while (0)



int result_shm_arr_create(long int **arr_ptr, const size_t shm_size);

void shm_close(long int *shm_arr, const size_t shm_size, const int shm_fd);

#endif