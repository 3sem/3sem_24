#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <sys/mman.h>
#include <sys/shm.h>
#include <unistd.h>
#include <fcntl.h>

#include "error.h"


static const key_t SHM_KEY = 0xDED007;


int
create_shm( const int n_pages)
{
    const int shmid = shmget( SHM_KEY, n_pages * getpagesize(),
                              0666 | IPC_CREAT);
    handle( shmid);
    
    return 0;
}

int
free_shm( const int n_pages)
{
    const int shmid = shmget( SHM_KEY, n_pages * getpagesize(), 0666);
    handle( shmid);
    handle( shmctl( shmid, IPC_RMID, NULL));
    
    return 0;
}

uintptr_t
get_shm( const int n_pages)
{
    const int shmid = shmget( SHM_KEY, n_pages * getpagesize(), 0666);
    handle( shmid);
    
    char *const shm = shmat( shmid, NULL, 0);
    handlep( shm);
    
    return (uintptr_t)shm;
}
