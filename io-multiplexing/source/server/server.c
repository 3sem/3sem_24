#include "../client/client.h"
#include "../common/log_utils.h"
#include "../common/file_utils.h"

#include "server.h"

#include <linux/limits.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

const char* success_message = "Registration successful\n";

void register_client(Clients* clients, const char* tx_path, const char* rx_path)
{
    if (clients->client_count >= MAX_CLIENTS){ LOG("Max client limit reached\n"); return; }

    if (mkfifo(tx_path, 0666) == -1){ LOG("Unable to create FIFO: %s\n", tx_path); return; }
    if (mkfifo(rx_path, 0666) == -1){ LOG("Unable to create FIFO: %s\n", rx_path); return; }

    LOG("Created FIFO's\n");

    int tx_fd = open(tx_path, O_RDWR);

    if (tx_fd == -1){ LOG("Unable to open: %s\n", tx_path); return; }
    
    int rx_fd = open(rx_path, O_RDWR);

    if (rx_fd == -1){ LOG("Unable to open: %s\n", rx_path); return; }

    #define current_client clients->clients[clients->client_count]

    current_client.tx_fd = tx_fd;
    strcpy(current_client.tx_path, tx_path); 

    current_client.rx_fd = rx_fd;
    strcpy(current_client.rx_path, rx_path); 

    #undef current_client

    clients->client_count++; 

    write(rx_fd, success_message, strlen(success_message));
    LOG("Client registered: T[%s], R[%s]\n", tx_path, rx_path);
}

void* handle_file_request(void* arg)
{
    Client* client = (Client*) arg;

    Buffer* buffer = create_buffer(BUFFER_SIZE);

    read_from_file(buffer, client->tx_fd);

    const char* filename = buffer->buffer;

    LOG("%s requested file: %s\n", client->tx_path, filename);

    int read_fd = open(filename, O_RDONLY);

    if (read_fd == -1) LOG("Unable to open: %s\n", filename);
    /*
    while (read_from_file(buffer, read_fd))
    {
        write_to_file(buffer, client->rx_fd);
    }
    */

    close(read_fd);

    destroy_buffer(buffer);
}

void handle_client_registration(Clients* clients, fd_set read_fds, int register_fd)
{
    if (FD_ISSET(register_fd, &read_fds))
    {
        Buffer* buffer = create_buffer(2 * PATH_MAX);

        read_from_file(buffer, register_fd);

        if (buffer->size > 0)
        {
            LOG("Reading from FIFO registration info... %s\n", buffer->buffer);

            char tx_path[PATH_MAX];
            char rx_path[PATH_MAX];

            if (sscanf(buffer->buffer, "REGISTER %s %s", tx_path, rx_path) == 2)
            {
                LOG("Registratng client...\n");

                register_client(clients, tx_path, rx_path);
            }
            else
            {
                LOG("Incorrect usage: REGISTER <fifo/tx/path> <fifo/rx/path>");
            }
        }
        else
        {
           // LOG("Nothing read from register fd\n");
        }

        destroy_buffer(buffer);
    }
}

void run_server(Clients* clients, int register_fd)
{
    fd_set read_fds;

    int max_fd = register_fd;

    while (1)
    {
        FD_ZERO(&read_fds);

        FD_SET(register_fd, &read_fds);

        for (int i = 0; i < clients->client_count; ++i)
        {
            FD_SET(clients->clients[i].tx_fd, &read_fds);

            if (clients->clients[i].tx_fd > max_fd)
            {
                max_fd = clients->clients[i].tx_fd;
            }
        }

        int activity = select(max_fd + 1, &read_fds, NULL, NULL, 0); // should the last be 0 instead of NULL?
        
        if (activity < 0) LOG("Activity error\n");

        handle_client_registration(clients, read_fds, register_fd);

        for (int i = 0; i < clients->client_count; i++) 
        {
            if (FD_ISSET(clients->clients[i].tx_fd, &read_fds)) 
            {
                pthread_t thread;
                pthread_create(&thread, NULL, handle_file_request, &clients[i]);
                pthread_detach(thread);
            }
        }
    }

    destroy_clients(clients);
}
