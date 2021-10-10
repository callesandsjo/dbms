#include "../include/options.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <signal.h>
#include <sys/time.h>
#include <fcntl.h>


int handle_options(int argc, char *argv[], int *port, char **logfile)
{
    int option;

    while ((option = getopt (argc, argv, ":p:l:dh")) != -1)
        switch (option)
        {
            case 'p':
                *port = atoi(optarg);
                break;
            case 'l':
                *logfile = optarg;
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
    umask(0);
    int pid;
    struct rlimit rl;
    struct sigaction sa;

    if(getrlimit(RLIMIT_NOFILE, &rl))
    {
        perror("Error creating daemon");
        exit(1);
    }
    if((pid = fork()) < 0)
    {
        perror("Couldnt create daemon");
        exit(1);
    }
    else if(pid != 0)
    {
        exit(0);
    }
    setsid();

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if(sigaction(SIGHUP, &sa, NULL) < 0)
    {
        perror("Error creating daemon");
        exit(1);
    }

    if(rl.rlim_max == RLIM_INFINITY)
        rl.rlim_max = 1024;
    
    for(int i = 0; i < rl.rlim_max; i++)
        close(i);
    
    int fd0 = open("/dev/null", O_RDWR);
    int fd1 = dup(0);
    int fd2 = dup(0);
    
}