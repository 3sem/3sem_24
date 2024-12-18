#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "incl/parse.h"

void parse(char *cmd, char **args) {
	int n = 0;
	char *arg = strtok(cmd, " \n");

	while (arg != NULL) {
		args[n] = arg;
		arg = strtok(NULL, " \n");
		n++;
	}

	args[n] = NULL;
}