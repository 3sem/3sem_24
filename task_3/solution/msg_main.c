#include <stdio.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "utils.h"

const char* input_filename = "../data/test_data.txt";

#define MSG_TYPE 1
const size_t msg_cap = 2048;

typedef struct
{
    long mtype;
    char mtext[msg_cap];
} msg;

const key_t MSG_ID = 777;

int main()
{
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
        alloc_buf(&buffer, LARGE_BUFFER_SIZE);

        int msg_d = msgget(MSG_ID, IPC_CREAT | 0666);
        if (msg_d == -1)
        {
            perror("parent msgget error ");
            return 1;
        }

        msg sbuf = { .mtype = MSG_TYPE, .mtext = {} };
        int input_fd = open_read_file(input_filename);
        if (input_fd == -1) return 1;

        ssize_t msg_len = 0;
        while ((msg_len = read_from(&buffer, input_fd)) > 0)
        {
            if (msg_len == -1)
            {
                perror("read error ");
                return 1;
            }

            size_t pos = 0;
            while (pos + msg_cap <= (size_t)buffer.size)
            {
                memcpy(sbuf.mtext, buffer.data + pos, msg_cap);
                pos += msg_cap;

                if (msgsnd(msg_d, &sbuf, msg_cap, 0) == -1)
                {
                    perror("msgsnd error ");
                    return 1;
                }
            }

            if (pos < (size_t)buffer.size)
            {
                size_t remaining_size = (size_t)buffer.size - pos;
                memcpy(sbuf.mtext, buffer.data + pos, remaining_size);
                if (msgsnd(msg_d, &sbuf, remaining_size, 0) == -1)
                {
                    perror("msgsnd error ");
                    return 1;
                }
            }
        }

        if (msgsnd(msg_d, &sbuf, 0, 0) == -1)
        {
            perror("msgsnd error ");
            return 1;
        }

        close(input_fd);
        dealloc_buf(&buffer);

        int status = 0;
        waitpid(pid, &status, 0);

        struct msqid_ds qstatus;
        msgctl(msg_d, IPC_RMID, &qstatus);

        time_check = clock() - time_check;
        printf("time: %ld\n", time_check);
    }
    else
    {
        int msg_d = msgget(MSG_ID, 0666);
        if (msg_d == -1)
        {
            perror("child msgget error ");
            return 1;
        }

        msg rbuf = {};
        int output_fd = open_write_file("output.txt");
        if (output_fd == -1) return 1;

        ssize_t msg_len = 0;
        while ((msg_len = msgrcv(msg_d, &rbuf, msg_cap, MSG_TYPE, 0)) > 0)
        {
            if (msg_len == -1)
            {
                perror("msgrcv error ");
                return 1;
            }

            if (write(output_fd, rbuf.mtext, (size_t)msg_len) < 0)
            {
                perror("write error ");
                return 1;
            }
        }

        close(output_fd);
    }

    return 0;
}
