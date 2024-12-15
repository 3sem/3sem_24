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

#define SIZE 1048576


typedef struct pPipe Pipe;
typedef struct op_table Ops;

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

typedef struct _file_t {
    int fd;
    size_t size;
} file_t;

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
        return read(p->fd_back[0], p->data, SIZE);
    } else {
        return read(p->fd_direct[0], p->data, SIZE);
    }
}

size_t parent_send(Pipe *p, const char *fname, file_t *file) {
    int fd = open(fname, O_RDONLY);
    if (fd == -1) {
        perror("Error open file");
        return (size_t)-1;
    }
    
    close(p->fd_direct[0]);
  
    
    ssize_t count;
    while ((count = read(fd, p->data, SIZE)) > 0) {
        p->len = count;
        if (p->actions.snd(p) != count) {
            perror("Error sending data to child");
            close(fd);
            return (size_t)-1;
        }
    }

    close(fd);
    close(p->fd_direct[1]);
    return 0;
}

size_t child_receive(Pipe *p, const char *fname, file_t *file) {
    file->fd = open(fname, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (file->fd == -1) {
        perror("Error opening output file");
        return (size_t)-1;
    }
    
    close(p->fd_direct[1]);
    
    ssize_t count;
    while ((count = p->actions.rcv(p)) > 0) {
        if (write(file->fd, p->data, count) != count) {
            perror("Error writing to file");
            close(file->fd);
            return (size_t)-1;
        }
    }

    close(p->fd_direct[0]);
    close(file->fd);
    return 0;
}

size_t child_send(Pipe *p, const char *fname, file_t *file) {
    int fd = open(fname, O_RDONLY);
    if (fd == -1) {
        perror("Error opening input file");
        return (size_t)-1;
    }

    close(p->fd_back[0]);
    
    ssize_t count;
    while ((count = read(fd, p->data, SIZE)) > 0) {
        p->len = count;
        if (p->actions.snd(p) != count) {
            perror("Error sending data to parent");
            close(fd);
            return (size_t)-1;
        }
       
    }

    close(fd);
    close(p->fd_back[1]);
    return 0;
}

size_t parent_receive(Pipe *p, const char *fname, file_t *file) {
    file->fd = open(fname, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (file->fd == -1) {
        perror("Error opening output file");
        return (size_t)-1;
    }

    close(p->fd_back[1]);
    
    ssize_t count;
    while ((count = p->actions.rcv(p)) > 0) {
        if (write(file->fd, p->data, count) != count) {
            perror("Error writing to file");
            close(file->fd);
            return (size_t)-1;
        }
    }

    close(p->fd_back[0]);
    close(file->fd);
    return 0;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s source_file dest_file\n", argv[0]);
        return 1;
    }

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    printf("Begin\n");

    Pipe *m = construct(SIZE);
    if (m == NULL) return 1;

    file_t file = {0, 0};
    const char *temp_file = "temp.txt";

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork error");
        free(m->data);
        free(m);
        return 1;
    } 
    else if (pid == 0) { 
        m->pid = 0;
        if (child_receive(m, temp_file, &file) == (size_t)-1) {
            exit(1);
        }
        if (child_send(m, temp_file, &file) == (size_t)-1) {
            exit(1);
        }
        unlink(temp_file);
        free(m->data);
        free(m);
        exit(0);
    } 
    else {  
        m->pid = pid;
        if (parent_send(m, argv[1], &file) == (size_t)-1) {
            return 1;
        }
        if (parent_receive(m, argv[2], &file) == (size_t)-1) {
            return 1;
        }
        
        waitpid(pid, NULL, 0);
        free(m->data);
        free(m);

        clock_gettime(CLOCK_MONOTONIC, &end);
        double time_spent = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
        printf("Time spent: %f seconds\n", time_spent);
        return 0;
    }
}