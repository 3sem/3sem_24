#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include <time.h>

#include "utils.h"

const char* input_filename = "../data/test_data.txt";
const char* fifo_name      = "test_fifo";


int main()
{
    unlink(fifo_name);

    if(mkfifo(fifo_name, 0666))
	{
        perror("bad mkfifo\n");
        return -1;
    }

    pid_t pid = fork();

    if (pid < 0)
	{
        perror("bad fork\n");
        return -1;
    }


    if (pid)
	{
		clock_t time_check = clock();

        Buffer buffer = {};
        alloc_buf(&buffer, CUR_BUFFER_SIZE);

        int write_fd = open(fifo_name, O_WRONLY);
        if (write_fd == -1)
		{
            perror("fifo open error");
            return -1;
        }

        int input_fd = open_read_file(input_filename);
        if (input_fd == -1) return -1;

        while (read_from(&buffer, input_fd))
		{
            write_to(&buffer, write_fd);
        }

        dealloc_buf(&buffer);
        close(write_fd);
        close(input_fd);

        waitpid(pid, NULL, 0);

        time_check = clock() - time_check;
        printf("time: %ld\n", time_check);
    }
    else
	{
        Buffer buffer = {};
        alloc_buf(&buffer, CUR_BUFFER_SIZE);

        int read_fd = open(fifo_name, O_RDONLY);
        if (read_fd == -1)
		{
            perror("open fifo");
            return -1;
        }

        int output_fd = open_write_file("output.txt");
        if (output_fd == -1) return -1;

        while (read_from(&buffer, read_fd))
		{
            write_to(&buffer, output_fd);
        }

        dealloc_buf(&buffer);
        close(read_fd);
        close(output_fd);
    }


    return 0;
}
