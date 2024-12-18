#ifndef SH_MEM_FUNC
#define SH_MEM_FUNC

#include "common_func.h"

#define SHM_NAME            "shared_mem_send"
//#define BUFF_SIZE           8192
#define FIRST_SEM_NAME      "snd_sem"
#define SECOND_SEM_NAME     "rcv_sem"

typedef struct 
{
    const char *shm_name;
    int         shm_fd;
    size_t      shm_size;
    char       *shm_map;
} shm_st;


int shm_init(shm_st *sh_memory_structure, const char *shm_name, const off_t shm_size);

int create_mapping(shm_st *sh_memory_structure);

int shm_destr(shm_st *sh_memory_structure);

int send_file_w_sh_memory(shm_st *shared_mem, const char *file_name);

int rcv_file_w_sh_memory(const char *rcv_file_name, shm_st *shared_mem);

void unlink_sems();

#endif