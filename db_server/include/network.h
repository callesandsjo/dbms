// This will handle the networking
#ifndef NETWORK_H
#define NETWORK_H

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h> 

int create_socket(uint16_t port);

void *handle_connection(void *p_client);

#endif
