// Main
#include "../include/network.h"
#include "../include/options.h"
#include "../include/logging.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#define TRUE 1
#define FALSE 0

int main(int argc, char *argv[])
{
    char *logfile = NULL;
    int port = 8080, mux = FALSE;
    char is_daemon = 0;

    if (handle_options(argc, argv, &port, &logfile, &is_daemon) == -1)
        exit(1);

    if(!logfile)
        printf("Server is up and running.\nPort: %d\nLogging to: Syslog\nRunning as daemon: %d\n", port, is_daemon);
    else
        printf("Server is up and running.\nPort: %d\nLogging to: %s.log.txt\nRunning as daemon: %d\n", port, logfile, is_daemon);

    if(is_daemon)
            run_as_daemon();

    int main_socket = create_socket(port);

    
    
    int address_length = sizeof(struct sockaddr_in); 
    struct sockaddr_in client_address;
    int client;

    if (mux)
    {
        return 0;
    }
    else // thread argument
    {
        pthread_t thread;
        struct thread_arguments args;
        if(!logfile)
        {
            args.logfile[0] = 0;
        }
        else
        {
            args.logfile[0] = 0;
            strcat(args.logfile,logfile);
            strcat(args.logfile,".log.txt");
            
        }

        

        while(1)
        {
            // threading
            args.client = accept(main_socket, (struct sockaddr *)&args.client_addr, (socklen_t*)&address_length);
            if(client == -1)
                exit(1);
            pthread_create(&thread, NULL, handle_connection, (void *)&args);
        }
    }
    close(main_socket);
    return 0;
}