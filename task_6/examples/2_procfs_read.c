#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void main(int argc, char **argv) {
    int pid;
    sscanf(argv[1], "%d", &pid);
    printf("pid = %d\n", pid);

    char filename[1000];
    sprintf(filename, "/proc/%d/stat", pid);
    FILE *f = fopen(filename, "r");

    int unused;
    char comm[1000];
    char state;
    int ppid;
    fscanf(f, "%d %s %c %d", &unused, comm, &state, &ppid);
    printf("comm = %s\n", comm);
    printf("state = %c\n", state);
    printf("parent pid = %d\n", ppid);
    fclose(f);
}
