#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "processor.h"

int main() {
    char* mem = calloc(MAX_LENGHT, sizeof(char));
    
    while (1) {
        fgets(mem, MAX_LENGHT, stdin);
        if (mem[strlen(mem) - 1] == '\n') mem[strlen(mem) - 1] = '\0';

        processed(mem);
    }

}