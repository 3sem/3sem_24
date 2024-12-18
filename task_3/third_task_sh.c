#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/time.h>

#define KEY 12345
#define BUFSIZE (1024 * 1024)
#define PERMS 0644
static double calculate_elapsed_time(struct timeval *start, struct timeval *end) {
    return (end->tv_sec - start->tv_sec) + 
           (end->tv_usec - start->tv_usec) / 1000000.0;
}


typedef struct {
    sem_t empty;
    sem_t full;
    sem_t mutex;
} sems_t;

typedef struct {
    ssize_t size;
    char *buf;
    int done;
} data_t;

void cleanup(data_t *data, sems_t *sems, int id1, int id2, int id3) {
    if (data && data->buf) shmdt(data->buf);
    if (data) shmdt(data);
    if (sems) {
        sem_destroy(&sems->empty);
        sem_destroy(&sems->full);
        sem_destroy(&sems->mutex);
        shmdt(sems);
    }
    
    if (id1 >= 0) shmctl(id1, IPC_RMID, NULL);
    if (id2 >= 0) shmctl(id2, IPC_RMID, NULL);
    if (id3 >= 0) shmctl(id3, IPC_RMID, NULL);
}

void receiver(char *outfile, data_t *data, sems_t *sems) {
    int fd = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, PERMS);
    if (fd < 0) {
        perror("open failed");
        return;
    }
    
    size_t total = 0;
    
    while (1) {
        sem_wait(&sems->full);
        sem_wait(&sems->mutex);
        
        if (write(fd, data->buf, data->size) < 0) {
            perror("write error");
            sem_post(&sems->mutex);
            sem_post(&sems->empty);
            close(fd);
            return;
        }
        
        total += data->size;
        printf("Got: %zu bytes", total);
        fflush(stdout);
        
        sem_post(&sems->mutex);
        sem_post(&sems->empty);

        if (!data->size || data->done) break;
    }
    
    close(fd);
}

void sender(char *infile, data_t *data, sems_t *sems) {
    int fd = open(infile, O_RDONLY);
    if (fd < 0) {
        perror("open failed");
        return;
    }
    
    size_t total = 0;
    data->done = 0;
    
    while (1) {
        sem_wait(&sems->empty);
        sem_wait(&sems->mutex);
        
        data->size = read(fd, data->buf, BUFSIZE);
        if (data->size < 0) {
            perror("read error");
            sem_post(&sems->mutex);
            sem_post(&sems->full);
            close(fd);
            return;
        }
        
        total += data->size;
        printf("Sent: %zu bytes", total);
        fflush(stdout);
        
        sem_post(&sems->mutex);
        sem_post(&sems->full);

        if (!data->size) {
            data->done = 1;
            break;
        }
    }
    
    close(fd);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <infile> <outfile>\n", argv[0]);
        return 1;
    }

    struct timeval t1, t2;
    gettimeofday(&t1, NULL);

    int id1 = shmget(KEY, sizeof(data_t), IPC_CREAT | PERMS);
    int id2 = shmget(KEY + 1, BUFSIZE, IPC_CREAT | PERMS);
    int id3 = shmget(KEY + 2, sizeof(sems_t), IPC_CREAT | PERMS);

    if (id1 < 0 || id2 < 0 || id3 < 0) {
        perror("shmget");
        return 1;
    }

    data_t *data = shmat(id1, NULL, 0);
    data->buf = shmat(id2, NULL, 0);
    sems_t *sems = shmat(id3, NULL, 0);

    if (sem_init(&sems->empty, 1, 1) < 0 ||
        sem_init(&sems->full, 1, 0) < 0 ||
        sem_init(&sems->mutex, 1, 1) < 0) {
        perror("sem_init");
        cleanup(data, sems, id1, id2, id3);
        return 1;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        cleanup(data, sems, id1, id2, id3);
        return 1;
    }

    if (!pid) {
        receiver(argv[2], data, sems);
        exit(0);
    }

    sender(argv[1], data, sems);
    wait(NULL);

    gettimeofday(&t2, NULL);
    double elapsed = calculate_elapsed_time(&t1, &t2);
    printf("Time: %.6f sec\n", elapsed);

    cleanup(data, sems, id1, id2, id3);
    return 0;
}
  
  
