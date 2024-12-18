#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
 
#define NIBBLE_0 SIGUSR1   
#define NIBBLE_1 SIGUSR2  
#define FINISH SIGTERM    
 
static volatile sig_atomic_t part = 0;
static volatile sig_atomic_t byte_ready = 0;
static volatile sig_atomic_t byte = 0;
static volatile sig_atomic_t is_high_part = 1;
static pid_t other_pid;
 
void handle_signals(int signo, siginfo_t *info, void *context) {
    int value = info->si_value.sival_int;
    
    if (signo == NIBBLE_0 && is_high_part) {
        byte = (value << 4);
        is_high_part = 0;
    } else if (signo == NIBBLE_1 && !is_high_part) {
        byte |= value;
        is_high_part = 1;
        byte_ready = 1;
    }
    
    if (kill(other_pid, SIGUSR1) == -1) {
        perror("kill");
        exit(EXIT_FAILURE);
    }
}
 
void handle_ack(int signo) {
    part = 0;
}
 
void handle_finish(int signo) {
    exit(EXIT_SUCCESS);
}
 
void setup_signal_handlers(int is_receiver) {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_flags = SA_SIGINFO;
    sigfillset(&sa.sa_mask);
 
    if (is_receiver) {
        sa.sa_sigaction = handle_signals;
        sigaction(NIBBLE_0, &sa, NULL);
        sigaction(NIBBLE_1, &sa, NULL);
        
        sa.sa_handler = handle_finish;
        sigaction(FINISH, &sa, NULL);
    } else {
        sa.sa_handler = handle_ack;
        sigaction(SIGUSR1, &sa, NULL);
    }
}
 
void send_byte(unsigned char b, pid_t receiver_pid) {
    union sigval value;
    sigset_t mask;
    sigemptyset(&mask);
    
    // Отправляем старшие 4 бита
    part = 1;
    value.sival_int = (b >> 4) & 0x0F;
    if (sigqueue(receiver_pid, NIBBLE_0, value) == -1) {
        perror("sigqueue");
        exit(EXIT_FAILURE);
    }
    
    while (part) {
        if (sigsuspend(&mask) == -1 && errno != EINTR) {
            perror("sigsuspend");
            exit(EXIT_FAILURE);
        }
    }
    
    // Отправляем младшие 4 бита
    part = 1;
    value.sival_int = b & 0x0F;
    if (sigqueue(receiver_pid, NIBBLE_1, value) == -1) {
        perror("sigqueue");
        exit(EXIT_FAILURE);
    }
    
    while (part) {
        if (sigsuspend(&mask) == -1 && errno != EINTR) {
            perror("sigsuspend");
            exit(EXIT_FAILURE);
        }
    }
}
 
void send_file(const char *filename, pid_t receiver_pid) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("open source");
        exit(EXIT_FAILURE);
    }
 
    unsigned char byte;
    while (read(fd, &byte, 1) == 1) {
        send_byte(byte, receiver_pid);
    }
 
    kill(receiver_pid, FINISH);
    close(fd);
}
 
void receive_file(const char *filename, pid_t sender_pid) {
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("open dest");
        exit(EXIT_FAILURE);
    }
 
    sigset_t mask;
    sigemptyset(&mask);
 
    while (1) {
        while (!byte_ready) {
            if (sigsuspend(&mask) == -1 && errno != EINTR) {
                perror("sigsuspend");
                close(fd);
                exit(EXIT_FAILURE);
            }
        }
 
        if (write(fd, &byte, 1) != 1) {
            perror("write");
            close(fd);
            exit(EXIT_FAILURE);
        }
 
        byte = 0;
        byte_ready = 0;
    }
 
    close(fd);
}
 
int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <source> <dest>\n", argv[0]);
        return 1;
    }
 
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigaddset(&mask, SIGUSR2);
    sigaddset(&mask, SIGTERM);
    sigprocmask(SIG_BLOCK, &mask, NULL);
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
 
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return 1;
    }
 
    if (pid == 0) {  
        other_pid = getppid();
        setup_signal_handlers(0);
        send_file(argv[1], other_pid);
    } else {        
        other_pid = pid;
        setup_signal_handlers(1);
        receive_file(argv[2], pid);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    double time_spent = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Time spent: %f seconds\n", time_spent);
 
    return 0;
}