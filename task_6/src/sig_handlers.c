#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include "debugging.h"
#include "config_changing_funcs.h"
#include "sig_handlers.h"

/*
В интерактивном режиме имеем 2 процесса: интерфейс и техническая часть
Соответственно тут приведены обработчики для соответствущих процессов
*/

static volatile sig_atomic_t technical_sigint_status   = 0;
static volatile sig_atomic_t interface_sigint_status   = 0;
static volatile sig_atomic_t interface_sigchld_status  = 0;

int signal_handler_set(handler_t (*handler)(int), const int signal_num)
{
    struct sigaction new_action = {};
    new_action.sa_handler = handler;
    new_action.sa_flags = (int)SA_RESETHAND;

    RETURN_ON_TRUE(sigaction(signal_num, &new_action, NULL) == -1, -1, perror("signal handler set error\n"););

    return 0;
}

handler_t technical_sigint(int signum)
{
    LOG("> child process finished\n");
    technical_sigint_status = signum;

    return;
}

[[noreturn]] handler_t interface_sigint(int signum)
{
    LOG("> interface was interupted");
    technical_sigint(SIGINT);

    interface_sigint_status = signum;

    destruct_cfg_fifo();

    _exit(SIGINT);
}

handler_t interface_sigchld(int signum)
{
    interface_sigchld_status = signum;

    return;
}

int check_interface_signals()
{
    RETURN_ON_TRUE(interface_sigchld_status, interface_sigchld_status);
    RETURN_ON_TRUE(interface_sigint_status, interface_sigint_status);

    return 0;
}

int check_technical_signals()
{
    RETURN_ON_TRUE(technical_sigint_status, technical_sigint_status);

    return 0;
}

//Сделать обработчик для интерфейса (в случае ошибки)