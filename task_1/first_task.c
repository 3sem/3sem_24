#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void exec_cmd(char ***cmd);
void free_t(char **str);
void free_tt(char ***str);
char **parse_line(char *string, int *num);
char ***parse_command(char **buf, int *num);

static const int BUFFER = 4096;

void free_tt(char ***str) {
  if (str == NULL) {
    return;
  }

  for (int i = 0; str[i] != NULL; i++) {
    for (int j = 0; str[i][j] != NULL; j++) {
      free(str[i][j]);
    }
    free(str[i]);
  }

  free(str);
}

void free_t(char **str) {
  if (str == NULL) {
    return;
  }
  int i;
  for (i = 0; str[i] != NULL; i++) {
    free(str[i]);
  }
  free(str);
}

char **parse_line(char *string, int *num) {
  if (string == NULL || num == NULL) {
    return NULL;
  }
  size_t len = strlen(string);
  if (len > 0 && string[len - 1] == '\n') {
    string[len - 1] = '\0';
  }
  char *str_copy = strdup(string);
  if (str_copy == NULL) {
    return NULL;
  }
  *num = 0;
  char *p = strtok(str_copy, "|");
  while (p != NULL) {
    (*num)++;
    p = strtok(NULL, "|");
  }
  free(str_copy);
  char **mass = (char **)calloc(*num + 1, sizeof(char *));
  if (mass == NULL) {
    return NULL;
  }
  int i = 0;
  p = strtok(string, "|");
  while (p != NULL && i < *num) {
    while (*p == ' ' || *p == '\t') p++;
    char *end = p + strlen(p) - 1;
    while (end > p && (*end == ' ' || *end == '\t')) end--;
    *(end + 1) = '\0';

    mass[i] = strdup(p);
    if (mass[i] == NULL) {
      free_t(mass);
      return NULL;
    }
    i++;
    p = strtok(NULL, "|");
  }
  mass[i] = NULL; 

  return mass;
}

char ***parse_command(char **buf, int *num) {
  if (buf == NULL || num == NULL || *num <= 0) {
    return NULL;
  }

  char ***command = (char ***)calloc(*num + 1, sizeof(char **));
  if (command == NULL) {
    return NULL;
  }

  for (int i = 0; i < *num; i++) {
    if (buf[i] == NULL) {
      free_tt(command);
      return NULL;
    }
    char *copy1 = strdup(buf[i]);
    if (copy1 == NULL) {
      free_tt(command);
      return NULL;
    }
    int word_count = 0;
    char *p = strtok(copy1, " \t\n");
    while (p != NULL) {
      word_count++;
      p = strtok(NULL, " \t\n");
    }
    free(copy1);
    command[i] = (char **)calloc(word_count + 1, sizeof(char *));
    if (command[i] == NULL) {
      free_tt(command);
      return NULL;
    }
    char *copy2 = strdup(buf[i]);
    if (copy2 == NULL) {
      free_tt(command);
      return NULL;
    }
    int j = 0;
    p = strtok(copy2, " \t\n");
    while (p != NULL && j < word_count) {
      command[i][j] = strdup(p);
      if (command[i][j] == NULL) {
        free(copy2);
        free_tt(command);
        return NULL;
      }
      j++;
      p = strtok(NULL, " \t\n");
    }
    command[i][j] = NULL;
    free(copy2);
  }
  command[*num] = NULL; 

  return command;
}

void exec_cmd(char ***cmd) {
  if (cmd == NULL) return;

  pid_t pid;
  int fds[2];
  int fd_in = 0;
  int i = 0;

  while (cmd[i] != NULL) {
    if (cmd[i + 1] != NULL) {
      if (pipe(fds) == -1) {
        perror("pipe failed");
        return;
      }
    }

    pid = fork();
    if (pid < 0) {
      perror("fork failed");
      return;
    } else if (pid == 0) {
      if (i > 0) {
        if (dup2(fd_in, STDIN_FILENO) == -1) {
          perror("dup2 failed");
          exit(1);
        }
        close(fd_in);
      }

      if (cmd[i + 1] != NULL) {
        if (dup2(fds[1], STDOUT_FILENO) == -1) {
          perror("dup2 failed");
          exit(1);
        }
        close(fds[1]);
        close(fds[0]);
      }

      execvp(cmd[i][0], cmd[i]);
      perror("execvp failed");
      exit(1);
    } else {
      if (i > 0) {
        close(fd_in);
      }

      if (cmd[i + 1] != NULL) {
        close(fds[1]);
        fd_in = fds[0];
      }

      int status;
      waitpid(pid, &status, 0);
      if (WIFEXITED(status)) {
        printf("Ret code: %d\n", WEXITSTATUS(status));
      }
      i++;
    }
  }

  if (fd_in != 0) {
    close(fd_in);
  }
}

int main() {
  while (1) {
    const int size = BUFFER;
    char string[size];
    printf("Enter command: ");
    if (fgets(string, size, stdin) == NULL) {
      return -1;
    }
   // printf("%s - our string\n", string);
    int a = 0;
    char **cmd = parse_line(string, &a);
    char ***cms = parse_command(cmd, &a);
  /*  for (int i = 0; i < a; i++) {
      printf("%s -stroka\n", cmd[i]);
    }*/
    exec_cmd(cms);
    free_t(cmd);
    free_tt(cms);
  }
  return 0;
}
