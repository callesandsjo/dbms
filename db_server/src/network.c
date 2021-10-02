#include "../include/network.h"


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
        char rt;
        char *error;
        if(handle_request(buf,&rt,error,client)) 
        {
            if (rt == RT_QUIT)
            {
                shutdown(client, SHUT_RDWR);
                close(client);   
                break;
            }    
        }
        //printf("Client message: %s", buf);
        memset(buf, 0, sizeof(buf));
        send(client, start, 3, 0);
    }
    puts("Thread terminated");
    fflush(stdout);
    return NULL;
}

bool handle_request(char * buf,char* request_type,char*error,int client)
{
    bool wasOk = false;
    request_t *req;
    req = parse_request(buf,&error);
    if (req != NULL)
    {
        wasOk = true;
        *request_type = req->request_type;
        if((int)*request_type == RT_CREATE)
        {
            //printf("creating table\n");
            if (create_table(req))
            {
                send(client,"Table created\n",14,0);
            }
            else
            {
                send(client,"Table already exists\n",21,0); 
            }
        }
        else if((int)*request_type == RT_TABLES)
        {
            char tables[1024] = "Current tables:\n";
            //printf("Searching for tables....\n");
            list_tables(tables);
            send(client, tables, strlen(tables),0);
        }
        else if ((int)*request_type == RT_SCHEMA)
        {
            char schemas[1024];
            list_schemas(schemas,req->table_name);
            send(client, schemas,strlen(schemas),0);
            memset(schemas,0,strlen(schemas));
        }
        else if((int)*request_type == RT_DROP)
        {
            drop_table(req->table_name);
        }
        else if((int)*request_type == RT_INSERT)
        {
            insert_record(req);
        }
        else if((int)*request_type == RT_SELECT)
        {
            char records[1024];
            select_record(req,records,2);
            //send(client, records,strlen(records),0);
        }
        else if ((int)*request_type != RT_QUIT)
        {
            printf("This command exists but is not implemented yet: %d!\n",request_type);
        }
        print_request(req); 
        destroy_request(req);
    }
    else
    {
        printf("error: %s\n",error);
        // const char *return_str = strcat(error, "\n");
        // send(client, return_str, strlen(return_str), 0);
        // free(error);
    }
    return wasOk;
}