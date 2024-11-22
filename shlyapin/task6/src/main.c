#include <stdio.h>
#include <stdlib.h>

#include "daemon.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Использование: %s <PID>\n", argv[0]);
        return 1;
    }

    printf("Starting daemon...\n\n");
    
    daemon_vasya(atoi(argv[1]));

    return 0;
}