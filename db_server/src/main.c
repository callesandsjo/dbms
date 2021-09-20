// Main
#include "../include/network.h"
#include "../include/options.h"
#include "../include/logging.h"
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    char *logfile = NULL;
    int port = 8080;

    if (handle_options(argc, argv, &port, &logfile) == -1)
        exit(1);
    
    db_log(logfile, logfile, LOG_INFO);

    int main_socket = create_socket(port);
    
    int address_length = sizeof(struct sockaddr_in); 
    struct sockaddr_in client_address;
    int client;

    while(1)
    {
        client = accept(main_socket, (struct sockaddr *)&client_address, (socklen_t*)&address_length);
        if(client == -1)
            exit(1);

        handle_connection(client);
    }
    close(main_socket);
    return 0;
}