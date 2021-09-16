#include "../include/network.h"
#include <stdlib.h>
#include <stdio.h>

int create_socket(uint16_t port)
{
    int _socket;
    struct sockaddr_in server_address;

    _socket = socket(AF_INET, SOCK_STREAM, 0);

    server_address.sin_addr.s_addr = INADDR_ANY;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(port);

    if (bind(_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
        exit(1);

    if (listen(_socket, 5) == -1)
        exit(1);
    
    printf("socket created");

    return _socket;
}


void handle_connection(int client)
{
    char buf[1024];
    size_t bytes_read;
    size_t message_size = 0;

    while((bytes_read = read(client, buf + message_size, sizeof(buf)-message_size-1)) != 0)
    {
        return;
    }
}
