#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <time.h>

int out_char = 0, counter = 128;
pid_t pid;


void childexit(int signo) {
  exit(EXIT_SUCCESS);
}

void parentexit(int signo) { 
  exit(EXIT_SUCCESS);
}

void empty(int signo) {
}

void one(int signo) {
  out_char += counter;
  counter /= 2; 
  kill(pid, SIGUSR1);
}

void zero(int signo) { 
  counter /= 2;   
  kill(pid, SIGUSR1);
}


int main(int argc, char ** argv) {
    if (argc != 2) {
        fprintf(stderr, "Use %s <input>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    pid_t ppid = getpid();

    sigset_t set;

    struct sigaction act_exit;
    memset(&act_exit, 0, sizeof(act_exit));
    act_exit.sa_handler = childexit; 
    sigfillset(&act_exit.sa_mask); 
    sigaction(SIGCHLD, &act_exit, NULL); 

    struct sigaction act_one;
    memset(&act_one, 0, sizeof(act_one));
    act_one.sa_handler = one;
    sigfillset(&act_one.sa_mask);
    sigaction(SIGUSR1, &act_one, NULL);

    struct sigaction act_zero;
    memset(&act_zero, 0, sizeof(act_zero));
    act_zero.sa_handler = zero;
    sigfillset(&act_zero.sa_mask);    
    sigaction(SIGUSR2, &act_zero, NULL);
 


    sigaddset(&set, SIGUSR1);
    sigaddset(&set, SIGUSR2);
    sigaddset(&set, SIGCHLD);
    sigprocmask(SIG_BLOCK, &set, NULL );
    sigemptyset(&set);


    pid = fork();

    struct timespec start, stop;
    clock_gettime(CLOCK_MONOTONIC, &start);

    if (pid == 0) {
        unsigned int fd = 0;
        char c = 0;
        sigemptyset(&set); 

    
        struct sigaction act_empty;                    
        memset(&act_empty, 0, sizeof(act_empty));
        act_empty.sa_handler = empty;
        sigfillset(&act_empty.sa_mask);    
        sigaction(SIGUSR1, &act_empty, NULL);

        struct sigaction act_alarm;
        memset(&act_alarm, 0, sizeof(act_alarm));
        act_alarm.sa_handler = parentexit;
        sigfillset(&act_alarm.sa_mask);
        sigaction(SIGALRM, &act_alarm, NULL);

    if ((fd = open(argv[1], O_RDONLY)) < 0 ) {
        perror("open error");
        exit(EXIT_FAILURE);
    }

    while (read(fd, &c, 1) > 0) {    
      alarm(1);

      for (int i = 128; i >= 1; i /= 2) {
            if ( i & c ) 
                kill(ppid, SIGUSR1);
            else      
                kill(ppid, SIGUSR2);
            sigsuspend(&set); 
        } 
    }
    exit(EXIT_SUCCESS);

    } else {

    int fdout = open("out.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    
    do {  
        if(counter == 0) {       
            write(fdout, &out_char, 1);        
            counter = 128;
            out_char = 0;
        }
        sigsuspend(&set);
    } while (1);
    wait(NULL);

    clock_gettime(CLOCK_MONOTONIC, &stop);
    double exec_time = (stop.tv_sec - start.tv_sec) + (stop.tv_nsec - start.tv_nsec) / (double) 1e9;
    printf("Время работы: %.4f с\n", exec_time); 
    
    exit(EXIT_SUCCESS);
    }
}