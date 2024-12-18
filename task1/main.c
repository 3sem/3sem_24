#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "incl/execcmd.h"
#include "incl/nums.h"
#include "incl/parse.h"

#define CMD_LEN 256

int main() {
	char cmd[CMD_LEN];

	printf("Enter command ('q' to quit)\n");

	while(1) {
		fgets(cmd, sizeof(cmd), stdin);

		if(strcmp(cmd, "q\n") == 0) {
			break;
		}
		
		exec_input_commands(cmd);
	}

	return 0;
}