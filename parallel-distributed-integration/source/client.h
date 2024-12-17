#ifndef CLIENT_H
#define CLIENT_H

#include "integral.h"
#include <arpa/inet.h>

struct _ServerInfo
{
    struct sockaddr_in address;
    int cores;
};

typedef struct _ServerInfo ServerInfo;

struct _Servers
{
    ServerInfo* server;
    int count;
    int total_cores;
};

typedef struct _Servers Servers;

Servers servers_create(int max_server_count);

void servers_destroy(Servers* servers);
int discover_servers(Servers* servers, int max_servers);

double communicate_task(ServerInfo* server, Task* task); 

double distribute_tasks(Servers* servers, Task* global_task);

#endif // !CLIENT_H
