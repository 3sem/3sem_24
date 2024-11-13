#include <stdio.h>
#include <semaphore.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <assert.h>
#include <memory.h>
#include "shared_mem_func.h"

static sem_t *sem_snd = NULL;
static sem_t *sem_rcv = NULL;

int shm_init(shm_st *sh_memory_structure, const char *shm_name, const off_t shm_size)
{
    assert(sh_memory_structure);
    assert(shm_name);

    int shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666);
    RETURN_ERROR_ON_TRUE(shm_fd == -1, -1,
        perror("shared mem open error"););

    int err_num = ftruncate(shm_fd, shm_size);
    RETURN_ERROR_ON_TRUE(err_num == -1, -1,
        perror("ftruncate call error occured"););

    sem_snd = sem_open(FIRST_SEM_NAME, O_CREAT | O_EXCL, 0666, 0);
    RETURN_ERROR_ON_TRUE(sem_snd == SEM_FAILED, -1, 
        perror("semaphore 1 creation failed");
        sem_unlink(FIRST_SEM_NAME);
        sem_unlink(SECOND_SEM_NAME););
    
    sem_rcv = sem_open(SECOND_SEM_NAME, O_CREAT | O_EXCL, 0666, 0);
    RETURN_ERROR_ON_TRUE(sem_rcv == SEM_FAILED, -1, 
        perror("semaphore 2 creation failed");
        sem_unlink(SECOND_SEM_NAME);
        sem_unlink(FIRST_SEM_NAME););

    sh_memory_structure->shm_name   = shm_name;
    sh_memory_structure->shm_fd     = shm_fd;
    sh_memory_structure->shm_size   = (size_t)shm_size;
    
    return 0;
}

int create_mapping(shm_st *sh_memory_structure)
{
    assert(sh_memory_structure);
    
    void *addr = mmap(NULL, sh_memory_structure->shm_size, PROT_WRITE | PROT_READ, MAP_SHARED, sh_memory_structure->shm_fd, 0);
    RETURN_ERROR_ON_TRUE(addr == MAP_FAILED, -1, 
        perror("mapping error"););

    sh_memory_structure->shm_map = (char *)addr;

    return 0;
}

int shm_destr(shm_st *sh_memory_structure)
{
    assert(sh_memory_structure);

    int err_num = 0;

    err_num = munmap(sh_memory_structure->shm_map, sh_memory_structure->shm_size);
    if (err_num)
        perror("unmapping error occured");
    
    close(sh_memory_structure->shm_fd);

    if (sem_close(sem_rcv) == -1)
        perror("semaphore 1 close error");
    if (sem_close(sem_snd) == -1)
        perror("semaphore 2 close error");

    sh_memory_structure->shm_map    = NULL;
    sh_memory_structure->shm_size   = 0;
    sh_memory_structure->shm_fd     = 0;

    if (sh_memory_structure->shm_name)
    {
        shm_unlink(sh_memory_structure->shm_name);
        sh_memory_structure->shm_name = NULL;
    }

    return err_num;
}

void unlink_sems()
{
    if (sem_unlink(FIRST_SEM_NAME) == -1)
        perror("first semaphore unlink error");

    if (sem_unlink(SECOND_SEM_NAME) == -1)
        perror("second semaphore unlink error");
    
}  

int send_file_w_sh_memory(shm_st *shared_mem, const char *file_name)
{
    assert(file_name);
    
    off_t   file_size   = 0;
    ssize_t err_num     = 0;
    
    int fd = open(file_name, O_RDONLY);
    RETURN_ERROR_ON_TRUE(fd == -1, -1,
        perror("file opening error"););

    file_size = get_file_size(fd);
    RETURN_ERROR_ON_TRUE(file_size == -1, -1);

    size_t bytes_left       = (size_t)file_size;
    memcpy(shared_mem->shm_map, &file_size, sizeof(size_t));
    sem_post(sem_rcv);

    while (bytes_left != 0)
    {
        size_t bytes_to_send = bytes_left > shared_mem->shm_size ? shared_mem->shm_size : bytes_left;

        LOG("> bytes to send: %ld\n", bytes_to_send);

        sem_wait(sem_snd);
        err_num = read(fd, shared_mem->shm_map, bytes_to_send);
        sem_post(sem_rcv);
        RETURN_ERROR_ON_TRUE(err_num == -1, -1,
            perror("read from file error");
            close(fd););

        LOG("bytes read: %ld\n", err_num);

        bytes_left -= bytes_to_send;
    }

    close(fd);

    return 0;
}


int rcv_file_w_sh_memory(const char *rcv_file_name, shm_st *shared_mem)
{
    assert(rcv_file_name);
    assert(shared_mem);

    int fd = open(rcv_file_name, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    RETURN_ERROR_ON_TRUE(fd == -1, -1,
        perror("rcv file opening error"););

    size_t  bytes_left  = 0;
    ssize_t err_num     = 0;

    sem_wait(sem_rcv);
    memcpy(&bytes_left, shared_mem->shm_map, sizeof(size_t));
    sem_post(sem_snd);

    LOG("> bytes left to read: %ld\n", bytes_left);

    while (bytes_left != 0)
    {
        size_t bytes_to_read = bytes_left > shared_mem->shm_size ? shared_mem->shm_size : bytes_left;

        LOG("> bytes need to read: %ld\n", bytes_left);

        sem_wait(sem_rcv);
        err_num = write(fd, shared_mem->shm_map, bytes_to_read);
        sem_post(sem_snd);
        RETURN_ERROR_ON_TRUE(err_num == -1, -1,
            perror("write into fd error");
            close(fd););

        bytes_left -= bytes_to_read;
    }

    close(fd);

    return fd;
}