#ifndef SIGHANDLERS
#define SIGHANLDERS

#include <stdio.h>

typedef void handler_t;

int signal_handler_set(handler_t (*handler)(int), const int signal_num);

handler_t technical_sigint(int signum);

handler_t interface_sigint(int signum);

handler_t interface_sigchld(int signum);

int check_interface_signals();

int check_technical_signals();

#endif