//-----------------------------------------------------------------
//
// Shared mamory file translation implementation
//
//-----------------------------------------------------------------

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include "../include/shared-memory.h"
#include "../include/define-check-condition-ret.h"

static const size_t SMEM_BUF_SIZE       = 1024;
static const char*  SMEM_TEMP_FILE_NAME = "/smem";

int smem_translate_file(int fd, size_t file_size) {
    key_t key  = ftok(SMEM_TEMP_FILE_NAME, 0);
    CHECK_CONDITION_PERROR_RET(key < 0,"ftok falilure", 1)

    int shm_id = shmget(key, file_size, 0666|IPC_CREAT);
    CHECK_CONDITION_PERROR_RET(shm_id == -1, "shget failure", 1)
    
    pid_t pid = fork();
    CHECK_CONDITION_PERROR_RET(pid < 0, "fork failure", 1);

    if (pid > 0) {
        char *buf = (char *) calloc(SMEM_BUF_SIZE, sizeof(char));
        CHECK_CONDITION_PERROR_RET(buf == NULL, "calloc", 1);

        //

        free(buf);

    } else {
        char *buf = (char *) calloc(SMEM_BUF_SIZE, sizeof(char));
        CHECK_CONDITION_PERROR_RET(buf == NULL, "calloc", 1);

        //

        free(buf);
        exit(1);
    }

    return 0;
}
