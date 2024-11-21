#ifndef INTERACTIVE_INTERFACE_HEADER
#define INTERACTIVE_INTERFACE_HEADER

#include <sys/types.h>

int run_interactive(const pid_t pid_to_monitor);

#define STANDART_PERIOD         10
#define STANDART_FILE_OUTPUT    1

#endif