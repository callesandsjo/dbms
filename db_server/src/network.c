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
    //int client = *((int*)p_client);
    struct thread_arguments test= *(struct thread_arguments*)(p_client);

    char buf[256];
    int bytes_read;

    puts("Created thread");
    const char *start = "$> ";
    send(test.client, start, 3, 0);
    handle_log(test,"Connection started",3);
    while((bytes_read = read(test.client, buf, sizeof(buf))) != 0)
    {
        char rt;
        char *error;
        if(handle_request(buf,&rt,error,test)) 
        {
            if (rt == RT_QUIT)
            {
                shutdown(test.client, SHUT_RDWR);
                close(test.client);
                handle_log(test,"Connection ended",3);   
                break;
            }    
        }
        //printf("Client message: %s", buf);
        memset(buf, 0, sizeof(buf));
        send(test.client, start, 3, 0);
    }
    puts("Thread terminated");
    fflush(stdout);
    return NULL;
}

bool handle_request(char * buf,char* request_type,char*error,struct thread_arguments args)
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
                char log[256] = "Table:'";
                strcat(log,req->table_name);
                strcat(log,"' created");
                send(args.client,"Table created\n",14,0);
                handle_log(args,log,3);
            }
            else
            {
                send(args.client,"Table already exists\n",21,0); 
            }
        }
        else if((int)*request_type == RT_TABLES)
        {
            char * tables = (char*)malloc(1024);
            strcpy(tables, "Current tables:\n");
            //printf("Searching for tables....\n");
            list_tables(tables);
            send(args.client, tables, strlen(tables),0);
            free(tables);
            handle_log(args,"Tables listed",3);
        }
        else if ((int)*request_type == RT_SCHEMA)
        {
            char * schemas = (char*)malloc(1024);
            schemas[0] = 0; 
            list_schemas(schemas,req->table_name);
            send(args.client, schemas,strlen(schemas),0);
            char log[256] = "Schemas from:'";
            strcat(log,req->table_name);
            strcat(log,"' gotten");
            handle_log(args,log,3);
            free(schemas);
        }
        else if((int)*request_type == RT_DROP)
        {
            drop_table(req->table_name);
            char log[256] = "Table:'";
            strcat(log,req->table_name);
            strcat(log,"' dropped");
            handle_log(args,log,3);
        }
        else if((int)*request_type == RT_INSERT)
        {
            insert_record(req);
            char log[256] = "Record inserted in ";
            strcat(log,req->table_name);
            handle_log(args,log,3);
        }
        else if((int)*request_type == RT_SELECT)
        {
            char records[1024] = "Records:\n";
            select_record(req,records,2);
            send(args.client, records,strlen(records),0);
            char log[256] = "Records from ";
            strcat(log,req->table_name);
            strcat(log," gotten");
            handle_log(args,log,3);
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
        handle_log(args,error,2);
        // const char *return_str = strcat(error, "\n");
        // send(client, return_str, strlen(return_str), 0);
        free(error);
    }
    return wasOk;
}
void handle_log (struct thread_arguments client,char * log_msg,int prio)
{
    char log[1024];
    log[0] = 0;
    char port_number[256];
    char ip_addr[INET_ADDRSTRLEN];
    //struct sockaddr_in pin;

    inet_ntop(AF_INET,&client.client_addr.sin_addr,ip_addr,sizeof(ip_addr));
    sprintf(port_number,"::%i - ",ntohs(client.client_addr.sin_port));

    timestamp(log);
    strcat(log," ");
    strcat(log,ip_addr);
    strcat(log,port_number);
    strcat(log,log_msg);
    strcat(log,"\n");

    db_log(T_LOG_PATH,log,prio);
}