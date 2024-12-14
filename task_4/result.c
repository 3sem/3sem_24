#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>

#define SHM_NAME "/integral_shm"


int main(int argc, char* argv[]) {
    int shm_fd = shm_open(SHM_NAME, O_RDONLY, 0666);
    if (shm_fd == -1) {
        perror("shm_open failed");
        return 1;
    }

    int N_threads = atoi(argv[1]);
    struct stat shm_stat;
    if (fstat(shm_fd, &shm_stat) == -1) {
        perror("fstat failed");
        close(shm_fd);
        return 1;
    }
    
    size_t shm_size = shm_stat.st_size;

    double *shared_mem = mmap(NULL, shm_size, PROT_READ, MAP_SHARED, shm_fd, 0);
    if (shared_mem == MAP_FAILED) {
        perror("mmap failed");
        close(shm_fd);
        return 1;
    }


 
    double total_result = 0.0;
    for (int i = 0; i < N_threads ; i++) {
        total_result += shared_mem[i];
        printf("Thread %d result: %.11f\n", i+1, shared_mem[i]);
    }
    
    printf("Final integral result: %.11f\n", total_result);
    
    munmap(shared_mem, shm_size);
    close(shm_fd);
   
    
    return 0;
}