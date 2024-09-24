#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

#include "duplex_pipe.h"

int     is_fd_open(int fd);
ssize_t pipe_recieve(Pipe *pp);
ssize_t pipe_send(Pipe *pp);
Pipe   *ctor_pipe();
int     dtor_pipe(Pipe *pp);

int is_fd_open(int fd) {
  return fcntl(fd, F_GETFD) == (-1) ? 0 : 1;
}

ssize_t pipe_recieve(Pipe *pp) {
  int work_fd = is_fd_open(pp->fd_direct[0]) ? pp->fd_direct[0] : pp->fd_back[0];

  return read(work_fd, pp->data, pp->len);
}

ssize_t pipe_send(Pipe *pp) {
  int work_fd = is_fd_open(pp->fd_direct[1]) ? pp->fd_direct[1] : pp->fd_back[1];

  return write(work_fd, pp->data, pp->len);
}

Pipe *ctor_pipe() {
  Pipe *pp = malloc(sizeof(Pipe));
  if (pp == NULL) {
    perror("Error malloc");
    return NULL;
  }

  pp->data = NULL;
  pp->len  = 0;

  if (pipe(pp->fd_direct)) {
    perror("Error create pipe");
    free(pp);
    return NULL;
  }
    
  if (pipe(pp->fd_back)) {
    perror("Error create pipe");
    free(pp);
    return NULL;
  }

  
  pp->actions.rcv = pipe_recieve;
  pp->actions.snd = pipe_send;

  return pp;
}

int dtor_pipe(Pipe *pp) {
  pp->data = NULL;
  pp->len  = 0;

  close(pp->fd_back[0]);
  close(pp->fd_back[1]);
  
  close(pp->fd_direct[0]);
  close(pp->fd_direct[1]);
  
  pp->actions.snd = NULL;
  pp->actions.rcv = NULL;

  free(pp);

  return 0;
}
