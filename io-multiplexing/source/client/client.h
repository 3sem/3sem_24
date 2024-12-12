#ifndef CLIENT_H
#define CLIENT_H

#include <linux/limits.h>

#define MAX_CLIENTS 64

struct _Client
{
    int tx_fd;
    int rx_fd;
    
    char tx_path[PATH_MAX];
    char rx_path[PATH_MAX];

};

typedef struct _Client Client;

struct _Clients 
{
    Client* clients;

    int client_count;

};

typedef struct _Clients Clients;

Clients* create_clients(int client_count);

void destroy_clients(Clients* clients);

#endif // !CLIENT_H
