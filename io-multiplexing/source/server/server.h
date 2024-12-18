#ifndef SERVER_H
#define SERVER_H

#include "../client/client.h"

#include <sys/select.h>

void* handle_file_request(void* arg);

void register_client(Clients* clients, const char* tx_path, const char* rx_path);

int handle_client_registration(Clients* clients, fd_set read_fds, int register_fd);

void run_server(Clients* clients, int register_fd);

#endif // !SERVER_H
