#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "integral.h"
#include "config.h"

typedef struct shared_data SharedData;
typedef struct shared_memory_config SharedMemoryConfig;

struct shared_memory_config {
    int shmid;
    SharedData *data;
};

struct shared_data {
    double sum;
    sem_t sem_lock;
};

enum error_codes {
    NOT_ERROR = 0,
    ERROR_SHMGET = 11,
    ERROR_SHMAT = 12,
    ERROR_SEM_INIT = 13
};

key_t generate_unique_key(void);
int initSharedMemory(SharedMemoryConfig *config);
ssize_t scatterPointsMonteCarlo(double left, double right, double *height, double (*f)(double), size_t npoints);
void computeDefiniteIntegralOnStreams(double a, double b, double (*f)(double), SharedData *data, int nstreams);



key_t generate_unique_key(void) {
    char temp_filename[] = "/tmp/somefileXXXXXX";
    int fd = mkstemp(temp_filename);
    if (fd == -1) {
        perror("Error creating temp file");
        return -1;
    }

    key_t key = ftok(temp_filename, 'a');
    close(fd);
    unlink(temp_filename);
    return key;
}

int initSharedMemory(SharedMemoryConfig *config) {
    key_t key = generate_unique_key();
    if (key < 0) {
        perror("Error ftok");
    }

    config->shmid = shmget(key, sizeof(SharedData), 0666 | IPC_CREAT);
    if (config->shmid < 0) {
        perror("Error shmget");
        return ERROR_SHMGET;
    }

    config->data = shmat(config->shmid, NULL, 0);
    if (config->data == (void *) -1) {
        perror("Error shmat");
        return ERROR_SHMAT;;
    }

    if (sem_init(&config->data->sem_lock, 1, 1) < 0) {
        perror("Error sem_init");
        return ERROR_SEM_INIT;
    }

    return NOT_ERROR;
}

ssize_t scatterPointsMonteCarlo(double left, double right, double *height, double (*f)(double), size_t npoints) {
    double max_fx = f(left);
    double min_fx = f(left);

    size_t num_pos_points = 0;
    size_t num_neg_points = 0;

    unsigned int seed = (unsigned int)time(NULL);

    int id = 0;
    do {
        if (id == 0) {
            id++;
        } else {
            *height *= 2;
        }

        num_pos_points = 0;
        num_neg_points = 0;

        for (size_t j = 0; j < npoints / 2; ++j) {
            double x = left + ((right - left) / RAND_MAX) * rand_r(&seed); // Случайное число от left до right
            double y = (*height / RAND_MAX) * rand_r(&seed); // Случайное число от 0 до height
            double fx = f(x);

            // Опрделение граничных значений
            max_fx = (fx > max_fx) ? fx : max_fx;
            min_fx = (fx < min_fx) ? fx : min_fx;

            if ((fx > 0) && ( y < fx)) { num_pos_points++; }
            if ((fx < 0) && (-y > fx)) { num_neg_points++; }
        }
    } while ((max_fx > *height) || (min_fx < -*height));

    return (ssize_t)(num_pos_points - num_neg_points);
}

void computeDefiniteIntegralOnStreams(double a, double b, double (*f)(double), SharedData *data, int nstreams) {
    pid_t *pids_child = calloc((size_t)nstreams, sizeof(pid_t));

    double step = (b - a) / nstreams;
    
    for (int i = 0; i < nstreams; ++i) {
        pids_child[i] = fork();
        
        if (pids_child[i] < 0) {
            perror("Error fork process");
            exit(EXIT_FAILURE);
        } else if (pids_child[i] == 0) {
            double left  = a + i * step;
            double right = left + step;
            double top   = 1;

            ssize_t sub_npoints = scatterPointsMonteCarlo(left, right, &top, f, (size_t)(MY_NPOINTS / nstreams));

            sem_wait(&data->sem_lock);
            data->sum += (2*top*step / ((double)MY_NPOINTS / nstreams)) * (double)(sub_npoints);
            sem_post(&data->sem_lock);
            
            shmdt(data);
            exit(EXIT_SUCCESS);
        }
    }

    shmdt(data);
    
    for (int i = 0; i < nstreams; ++i) {
        int status;
        waitpid(pids_child[i], &status, 0);

        if (WEXITSTATUS(status) != EXIT_SUCCESS) {
            perror("Error waitpid");
            exit(EXIT_FAILURE);
        }
    }

    free(pids_child);

    exit(EXIT_SUCCESS);
}


double computeDefiniteIntegral(double a, double b, double (*f)(double)) {
    if (a > b) {
        double tmp = a;
        a = b;
        b = tmp;
    }
   
    SharedMemoryConfig config;
    if (initSharedMemory(&config) != NOT_ERROR) {
        perror("Error initSharedMemory");
        return NAN;
    }

    pid_t pid = fork();
    if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);
        if (WEXITSTATUS(status) != EXIT_SUCCESS) {
            perror("Error waitpid");
            exit(EXIT_FAILURE);
        }

        double res = config.data->sum;

        shmdt(config.data);
        shmctl(config.shmid, IPC_RMID, NULL);
        
        return res;
    } else if (pid == 0) {
        computeDefiniteIntegralOnStreams(a, b, f, config.data, MY_NSTREAMS);
    } else {
        perror("Error fork process");
        exit(EXIT_FAILURE);
    }

    return NAN;
}

double testComputeDefiniteIntegral(double a, double b, double (*f)(double), int nstreams) {
    if (a > b) {
        double tmp = a;
        a = b;
        b = tmp;
    }

    SharedMemoryConfig config;
    if (initSharedMemory(&config) != NOT_ERROR) {
        perror("Error initSharedMemory");
        return NAN;
    }

    pid_t pid = fork();
    if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);
        if (WEXITSTATUS(status) != EXIT_SUCCESS) {
            perror("Error waitpid");
            exit(EXIT_FAILURE);
        }

        double res = config.data->sum;

        shmdt(config.data);
        shmctl(config.shmid, IPC_RMID, NULL);
        
        return res;
    } else if (pid == 0) {
        computeDefiniteIntegralOnStreams(a, b, f, config.data, nstreams);
    } else {
        perror("Error fork process");
        exit(EXIT_FAILURE);
    }

    return NAN;
}
