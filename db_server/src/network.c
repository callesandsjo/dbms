#include "../include/network.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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

    printf("Socket created\n");

    return _socket;
}


void *handle_connection(void *p_client)
{
    // function to handle each clients connection (Gateway)
    int client = *((int*)p_client);

    char buf[256];
    int bytes_read;

    puts("Created thread");
    const char *start = "$> ";
    send(client, start, 3, 0);

    while((bytes_read = read(client, buf, sizeof(buf))) != 0)
    {
        
        request_t *req;
        char *error;
        
        req = parse_request(buf, &error);
        if(req != NULL) {
            
            char type = req->request_type;
            char *return_str;

            printf("type: %d\n", req->request_type);
            printf("table_name: %s\n", req->table_name);
            print_request(req);
            
            destroy_request(req);

            if (type == 6)
            {
                return_str = "Exiting ...\n";
                send(client, return_str, strlen(return_str), 0);
                shutdown(client, SHUT_RDWR);
                close(client);
                break;   
            }
            else
            {
                return_str = "This command exists but is not implemented yet!\n";
                send(client, return_str, strlen(return_str), 0);
            }    
        }
        else {
            printf("%s\n", error);
            const char *return_str = strcat(error, "\n");
            send(client, return_str, strlen(return_str), 0);
            free(error);
        }
        memset(buf, 0, sizeof(buf));
        send(client, start, 3, 0);
    }
    puts("Thread terminated");
    fflush(stdout);
    return NULL;
}
