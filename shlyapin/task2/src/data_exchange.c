#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "duplex_pipe.h"
#include "data_exchange.h"

#define BUF_SIZE (1024*1024)

int forward_file_to_pipe(Pipe *pp, const char *fname);
int forward_pipe_to_file(Pipe *pp, const char *fname);

int data_exchange_file(char **fnames) {
  Pipe *pp = ctor_pipe();
  if (!pp) {
    perror("Error create Pipe");
    return ERROR_CREATE_PIPE;
  }

  pid_t pid = fork();
  if (pid < 0) {
    perror("Error call fork");
    dtor_pipe(pp);
    return ERROR_CALL_FORK;
  }

  if (pid > 0) {
    close(pp->fd_direct[0]);
    close(pp->fd_back[1]);
    
    forward_file_to_pipe(pp, fnames[0]);
    close(pp->fd_direct[1]);
    
    int status;
    wait(&status);
  } else if (pid == 0) {
    close(pp->fd_direct[1]);
    close(pp->fd_back[0]);
    
    forward_pipe_to_file(pp, fnames[1]);
  }

  dtor_pipe(pp);
  
  return 0;
}

int forward_pipe_to_file(Pipe *pp, const char *fname) {
  pp->len  = BUF_SIZE;
  pp->data = malloc(BUF_SIZE);
  if (!pp->data) {
    perror("Error malloc");
    exit(EXIT_FAILURE);
  }

  int fd = open(fname, O_WRONLY | O_CREAT);
  if (fd == -1) {
    perror("Error open file");
    free(pp->data);
    exit(EXIT_FAILURE);
  }

  ssize_t count = pp->actions.rcv(pp);
  while (count) {
    if (count == -1) {
      perror("Error write in file");
      break;
    }
    write(fd, pp->data, count);
    count = pp->actions.rcv(pp);
  }
  
  free(pp->data);
  
  close(fd);  
  
  return 0;
}

int forward_file_to_pipe(Pipe *pp, const char *fname) {
  pp->data = malloc(BUF_SIZE);
  if (!pp->data) {
    perror("Error malloc");
    exit(EXIT_FAILURE);
  }
  
  int fd = open(fname, O_RDONLY);
  if (fd == -1) {
    perror("Error open file");
    free(pp->data);
    exit(EXIT_FAILURE);
  }

  int count = read(fd, pp->data, BUF_SIZE);
  while (count) {
    if (count == -1) {
      perror("Error read from file");
      break;
    }

    pp->len = count;
    if (pp->actions.snd(pp) == -1) {
      perror("Error write in pipe");
      break;
    }

    count = read(fd, pp->data, BUF_SIZE);
  }

  close(fd);
  free(pp->data);

  return 0;
}
