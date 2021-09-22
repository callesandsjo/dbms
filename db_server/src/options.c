#include "../include/options.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int handle_options(int argc, char *argv[], int *port, char **logfile)
{
    int option;

    while ((option = getopt (argc, argv, ":p:l:dh")) != -1)
        switch (option)
        {
            case 'p':
                *port = atoi(optarg);
                printf("port: %d\n", *port);
                break;
            case 'l':
                *logfile = optarg;
                printf("logfile: %s\n", *logfile);
                break;
            case 'd':
                run_as_daemon();
                break;
            case 'h':
                help();
                break;
            case ':':
                fprintf (stderr, "Option -%c requires an argument.\n", optopt);
                help();
                return -1;
            case '?':
                fprintf (stderr,
                        "Unknown option\n");
                help();
                return -1;
            default:
                help();
                exit(1);
        }
    return 0;
}

void help()
{
    const char *help_text = "\
Usage: ./db_server [-h] [-p port] [-d] [-l logfile]\n\
where:\n\
    -h  show help text\n\
    -p  change port (default: 8080)\n\
    -d  run the server as daemon\n\
    -l  log to the specified logfile (default: logging to syslog)";
    
    printf("%s", help_text);
    exit(0);
}

void run_as_daemon()
{
    printf("Running server as daemon");
}