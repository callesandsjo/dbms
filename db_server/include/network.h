// This will handle the networking
#ifndef NETWORK_H
#define NETWORK_H
#include "../lib/request.h"
#include "../include/tablehandler.h"
#include "../include/logging.h"
#include <stdbool.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h> 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
struct thread_arguments
{
    int client;
    struct sockaddr_in client_addr;
};


int create_socket(uint16_t port);

void *handle_connection(void *p_client);

bool handle_request(char * buf,char* request_type,char*error,struct thread_arguments args);
void handle_log (struct thread_arguments client,char * log_msg,int prio);
#endif
