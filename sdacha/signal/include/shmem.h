#ifndef SHMEM_H
#define SHMEM_H


#include <stdint.h>

int       create_shm( const int n_pages);
int         free_shm( const int n_pages);
uintptr_t    get_shm( const int n_pages);


#endif // SHMEM_H
