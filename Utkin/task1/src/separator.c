#include <stdio.h>
#include <string.h>
#include "separator.h"


int separator(char* start, char** param, char* delim) {
    int kol = 0;
    char* t = strtok(start, delim);

    while (t != NULL) {
        param[kol] = t;
        t = strtok(NULL, delim);
        kol++;
    }

    param[kol] = NULL;
    
    if (kol == 0) {param[0] = start; param[1] = NULL; kol++;}

    return kol;
}