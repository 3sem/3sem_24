#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <syslog.h>

#define SIZE 4096
#define MAXLINE 1024

// Предварительные объявления структур
typedef struct pPipe Pipe;
typedef struct op_table Ops;

// Предварительные объявления функций
size_t receive(Pipe *p);
size_t send(Pipe *p);
size_t child(Pipe *p, const char *fname);
size_t parent(Pipe *p, const char *fname);

struct op_table {
    size_t (*rcv)(Pipe *self);
    size_t (*snd)(Pipe *self);
};

struct pPipe {
    char* data;
    int fd_direct[2];
    int fd_back[2];
    size_t len;
    Ops actions;
    pid_t pid;
};

Pipe *construct(int size) {
    Pipe *p = malloc(sizeof(Pipe));
    if (p == NULL) {
        perror("error malloc");
        return NULL;
    }
    
    if (pipe(p->fd_direct) == -1) {
        perror("Error pipe");
        free(p);
        return NULL;
    }
    
    if (pipe(p->fd_back) == -1) {
        perror("Error pipe");
        free(p);
        return NULL;
    }
    
    p->data = malloc(size);
    if (p->data == NULL) {
        perror("Error malloc for data");
        free(p);
        return NULL;
    }
    
    p->len = 0;
    p->actions.rcv = receive;
    p->actions.snd = send;
    p->pid = 0;
    return p;
}

size_t send(Pipe *p) {
    if (p->pid > 0) {
        return write(p->fd_direct[1], p->data, p->len);
    } else {
        return write(p->fd_back[1], p->data, p->len);
    }
}

size_t receive(Pipe *p) {
    if (p->pid > 0) {
        return read(p->fd_back[0], p->data, p->len);
    } else {
        return read(p->fd_direct[0], p->data, p->len);
    }
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s source_file dest_file\n", argv[0]);
        return 1;
    }

    clock_t start = clock();
    printf("begin\n");
    
    Pipe *m = construct(SIZE);
    if (m == NULL) {
        return 1;
    }
    
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork error");
        free(m->data);
        free(m);
        return 1;
    } else if (pid == 0) {
        m->pid = 0;
        close(m->fd_direct[1]);
        close(m->fd_back[0]);
        child(m, argv[2]);
        exit(0);
    } else {
        m->pid = pid;
        close(m->fd_direct[0]);
        close(m->fd_back[1]);
        parent(m, argv[1]);
        waitpid(pid, NULL, 0);
        
        free(m->data);
        free(m);
        
        clock_t end = clock();
        double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
        printf("Time spent: %f seconds\n", time_spent);
        return 0;
    }
}

size_t child(Pipe *p, const char *fname) {
    p->len = SIZE;
    
    int fd = open(fname, O_WRONLY | O_CREAT, 0644);
    if (fd == -1) {
        perror("Error open file");
        return (size_t)-1;
    }

    ssize_t count = p->actions.rcv(p);
    while (count > 0) {
        if (write(fd, p->data, (size_t)count) == -1) {
            perror("Error write in file");
            close(fd);
            return (size_t)-1;
        }
        count = p->actions.rcv(p);
    }

    close(fd);
    return 0;
}

size_t parent(Pipe *p, const char *fname) {
    int fd = open(fname, O_RDONLY);
    if (fd == -1) {
        perror("Error opening input file");
        return (size_t)-1;
    }

    ssize_t count;
    while ((count = read(fd, p->data, SIZE)) > 0) {
        p->len = (size_t)count;
        if (p->actions.snd(p) == -1) {
            perror("Error write in pipe");
            close(fd);
            return (size_t)-1;
        }
    }

    if (count == -1) {
        perror("Error read from file");
        close(fd);
        return (size_t)-1;
    }

    close(fd);
    return 0;
}
