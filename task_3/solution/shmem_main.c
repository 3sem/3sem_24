#include <sys/types.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "utils.h"

const char* input_filename = "../data/test_data.txt";

const key_t SHMEM_ID = 777;

const char* p_sem_name = "p_sem";
const char* c_sem_name = "c_sem";

int main()
{
    sem_unlink(p_sem_name);
    sem_unlink(c_sem_name);

    sem_t* parent_sem = sem_open(p_sem_name, O_CREAT, S_IRUSR | S_IWUSR, 1);
    if (parent_sem == SEM_FAILED)
	{
        perror("parent parent_sem sem_open error ");
        return 1;
    }

    sem_t* child_sem = sem_open(c_sem_name, O_CREAT, S_IRUSR | S_IWUSR, 0);
    if (child_sem == SEM_FAILED)
	{
        perror("parent child_sem sem_open error ");
        return 1;
    }

    pid_t pid = fork();
    if (pid == -1)
	{
        perror("bad fork");
        return 1;
    }

    if (pid)
	{
		clock_t time_check = clock();

        Buffer buffer = {};
        alloc_buf(&buffer, CUR_BUFFER_SIZE);

        size_t shmem_size = CUR_BUFFER_SIZE + sizeof(size_t);

        int shmem_id = shmget(SHMEM_ID, shmem_size, IPC_CREAT | 0666);
        if (shmem_id == -1)
		{
            perror("shmget error ");
            return 1;
        }

        int input_fd = open_read_file(input_filename);
        if (input_fd == -1) return 1;

        char* shmaddr = shmat(shmem_id, NULL, 0);
        if (shmaddr == (void*) -1)
		{
            perror("bad shmat");
            return 1;
        }

        while (read_from(&buffer, input_fd))
		{
            sem_wait(parent_sem);
            memcpy(shmaddr, &(buffer.size), sizeof(size_t));
            memcpy(shmaddr + sizeof(size_t), buffer.data, buffer.capacity);
            sem_post(child_sem);
        }

        sem_wait(parent_sem);

        size_t* aligned_addr =
			(size_t*) (((uintptr_t) shmaddr + sizeof(size_t) - 1) & ~(sizeof(size_t) - 1));
		*aligned_addr = 0;

        sem_post(child_sem);

        close(input_fd);
        dealloc_buf(&buffer);
        sem_close(parent_sem);

        waitpid(pid, NULL, 0);

		time_check = clock() - time_check;
        printf("time: %ld\n", time_check);
    }
    else
	{
        size_t shmem_size = CUR_BUFFER_SIZE;

        int shmem_id = shmget(SHMEM_ID, shmem_size, IPC_CREAT | 0666);
        if (shmem_id == -1)
		{
            perror("Child shmget error ");
            return 1;
        }

        char* shmaddr = shmat(shmem_id, NULL, 0);
        if (shmaddr == (void*) -1)
		{
            perror("bad shmat");
            return 1;
        }

        int output_fd = open_write_file("output.txt");
        if (output_fd == -1) return 1;

        while (1)
		{
            sem_wait(child_sem);

            size_t* aligned_addr =
				(size_t*) (((uintptr_t) shmaddr + sizeof(size_t) - 1) & ~(sizeof(size_t) - 1));
			if (*aligned_addr == 0) break;

            size_t data_size = 1;
			memcpy(&data_size, shmaddr, sizeof(size_t));

			if (write(output_fd, shmaddr + sizeof(size_t), data_size) < 0) {
				perror("write error ");
				return 1;
			}


            sem_post(parent_sem);
        }

        close(output_fd);
        sem_close(child_sem);

        struct shmid_ds shmid_ds;
        if (shmctl(shmem_id, IPC_RMID, &shmid_ds) == -1)
		{
            perror("Child shmctl ");
            return 1;
        }
    }

    return 0;
}
