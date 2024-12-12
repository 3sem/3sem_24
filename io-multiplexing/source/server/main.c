#include "server.h"
#include "../common/log_utils.h"
#include "../client/client.h"

#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

int main()
{
    const char* register_fifo = "register";

    mkfifo(register_fifo, 0666);

    int register_fd = open(register_fifo, O_RDONLY);

    if (register_fd < 0) { LOG("Unable to open FIFO: %s", register_fifo); return -1; }

    LOG("Server running...\n");

    Clients* clients = create_clients(MAX_CLIENTS);

    run_server(clients, register_fd);

    destroy_clients(clients);

    close(register_fd);

    unlink(register_fifo);
}
