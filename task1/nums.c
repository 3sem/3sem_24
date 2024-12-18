#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "incl/nums.h"

int num(char *cmd, char **commands) {
	int commands_num = 0;

	char *token = strtok(cmd, "|");
	while (token != NULL) {
		commands[commands_num] = token;
		token = strtok(NULL, "|");
		commands_num++;
	}

	return commands_num;
}