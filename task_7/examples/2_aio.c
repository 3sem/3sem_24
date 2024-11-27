#include <aiocb.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void aioSigHandler(int sig, siginfo_t *si, void *ucontext) {
    if (si->si_code == SI_ASYNCIO) {
        printf("AIO completion signal received\n");
        // Check the status of the AIO request
        int error = aio_error(&aiocb);
        if (error == EINPROGRESS) {
            printf("AIO request still in progress\n");
        } else if (error == 0) {
            int bytes_read = aio_return(&aiocb);
            printf("AIO read completed: %d bytes read\n", bytes_read);
            // Handle the read data
            buffer[bytes_read] = '\0';
            printf("Read: %s\n", buffer);
        } else {
            perror("aio_error");
        }
    }
}

int main() {
    struct aiocb aiocb;
    char buffer[1024];

    // Initialize the AIO control block
    aiocb.aio_fildes = STDIN_FILENO; // File descriptor for stdin
    aiocb.aio_offset = 0; // Offset within the file (not relevant for stdin)
    aiocb.aio_buf = buffer; // Buffer to store the read data
    aiocb.aio_nbytes = 1024; // Size of the buffer
    aiocb.aio_reqprio = 0; // Priority of the request (0 for no priority)

    struct sigevent sigevent;
    sigevent.sigev_notify = SIGEV_SIGNAL;
    sigevent.sigev_signo = SIGUSR1; // Signal to be sent on completion
    sigevent.sigev_value.sival_ptr = &aiocb; // Value to be passed with the signal

    aiocb.aio_sigevent = sigevent;

    // Register the signal handler
    struct sigaction sa;
    sa.sa_sigaction = aioSigHandler;
    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    // Initiate the AIO read
    if (aio_read(&aiocb) == -1) {
        perror("aio_read");
        exit(1);
    }

    // Wait for the signal or perform other tasks
    while (1) {
        pause();
    }

    return 0;
}
