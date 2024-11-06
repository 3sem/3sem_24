//-----------------------------------------------------------------
//
// Shared mamory file translation implementation
//
//-----------------------------------------------------------------

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
// #include <semaphore.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include "../include/shared-memory.h"
#include "../include/define-check-condition-ret.h"
#include "../include/buf-size.h"
#include "../config/config.h"

static const size_t SMEM_SIZE           = FILESENDING_BUF_SIZE;
static const size_t SMEM_NUM_PARTS      = 4;

static struct smem_args {
    int fd;
    void *smem;
    size_t file_size;
    size_t num;
    pthread_mutex_t mutex;
    pthread_cond_t producer;
    pthread_cond_t consumer;
};

#if 0
static void print_args(struct smem_args *args) {
    printf("fd: %d\n", args->fd);
    printf("smem: %p\n", args->smem);
    printf("file_size: %lu\n", args->file_size);
    printf("mutex: %p\n", &args->mutex);
    printf("producer: %p\n", &args->producer);
    printf("consumer: %p\n", &args->consumer);
}
#endif 

static void *smem_recv(void *v_args) {
    struct smem_args *args = (struct smem_args *) v_args;

    char *buf = malloc(SMEM_SIZE);
    size_t char_rcvd = 0;
    size_t add = 0;

    int fd = open(SMEM_RCVD_FILE_NAME, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    CHECK_CONDITION_PERROR_RET(fd == -1, "open", NULL);

    while (1) {
        pthread_mutex_lock(&args->mutex);
        if (args->num == 0) {
            pthread_cond_wait(&args->consumer, &args->mutex);
        }

        memcpy(buf, args->smem + SMEM_SIZE * add, SMEM_SIZE);

        size_t char_write = write(fd, buf, SMEM_SIZE);
        CHECK_CONDITION_PRINT(char_write != SMEM_SIZE);

        add = (add + 1) % SMEM_NUM_PARTS;
        args->num -= 1;

        char_rcvd += SMEM_SIZE;

        if (char_rcvd >= args->file_size) {
            break;
        }

        pthread_cond_signal(&args->producer);

        pthread_mutex_unlock(&args->mutex);
    }

    pthread_cond_signal(&args->producer);
    pthread_mutex_unlock(&args->mutex);

    free(buf);

    int *result = (int *) malloc(sizeof(int));
    *result = 1;

    if (char_rcvd >= args->file_size) {
        *result = 0;
    }

    close(fd);

    return result;
}

static void *smem_send(void *v_args) {
    struct smem_args *args = (struct smem_args *) v_args;

    size_t char_read = 0;
    size_t char_sent = 0;
    size_t rem = 0;

    while (1) {
        pthread_mutex_lock(&args->mutex);
        if (args->num == SMEM_NUM_PARTS) {
            pthread_cond_wait(&args->producer, &args->mutex);
        }

        if ((char_read = read(args->fd, args->smem + SMEM_SIZE * rem, SMEM_SIZE)) <= 0) {
            break;
        }

        rem = (rem + 1) % SMEM_NUM_PARTS;
        args->num += 1;

        char_sent += char_read;

        if (char_sent >= args->file_size) {
            break;
        }

        pthread_cond_signal(&args->consumer);
        pthread_mutex_unlock(&args->mutex);
    }

    pthread_cond_signal(&args->consumer);
    pthread_mutex_unlock(&args->mutex);

    return NULL;
}

int smem_translate_file(int fd, size_t file_size) {
    int smem_fd = shm_open(SMEM_TEMP_FILE_NAME, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    CHECK_CONDITION_PERROR_RET(smem_fd == -1, "shm_open", 1)

    int  ftruncate_res = ftruncate(smem_fd, SMEM_SIZE * SMEM_NUM_PARTS);
    CHECK_CONDITION_PERROR_RET(ftruncate_res == -1, "ftruncate", 1)

    void *smem = mmap(NULL, SMEM_SIZE * SMEM_NUM_PARTS, PROT_READ | PROT_WRITE, MAP_SHARED, smem_fd, 0);
    CHECK_CONDITION_PERROR_RET(smem == NULL, "mmap", 1)

    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);

    pthread_cond_t c_producer = PTHREAD_COND_INITIALIZER;
    pthread_cond_t c_consumer = PTHREAD_COND_INITIALIZER;
    struct smem_args args;
    args.fd        = fd;
    args.smem      = smem;
    args.file_size = file_size;
    args.num       = 0;
    args.mutex     = mutex;
    args.producer  = c_producer;
    args.consumer  = c_consumer;

    pthread_t id1, id2;
    pthread_create(&id1, NULL, smem_send, &args);
    pthread_create(&id2, NULL, smem_recv, &args);

    void *rcvd_res;
    pthread_join(id2, &rcvd_res);
    pthread_join(id1, NULL);

    CHECK_CONDITION_PERROR_RET(munmap(smem, SMEM_SIZE) == -1, "munmap", 1)
    CHECK_CONDITION_PERROR_RET(close(smem_fd) == -1, "close", 1)
    CHECK_CONDITION_PERROR_RET(shm_unlink(SMEM_TEMP_FILE_NAME) == -1, "shm_unlink", 1)

    if ((*((int *)(rcvd_res))) == 0) {
        free(rcvd_res);
        return 0;
    }

    free(rcvd_res);

    return 1;
}
