#ifndef INTERACTIVE_INTERFACE_HEADER
#define INTERACTIVE_INTERFACE_HEADER

#include <sys/types.h>

//Разбиваем программу на два процесса: интерфейс и функциональные процессы
int run_interactive(const pid_t pid_to_monitor);

#endif