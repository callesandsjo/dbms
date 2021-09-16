#include "../include/logging.h"
#include <stdio.h>


int setup_logging(char *logfile){
    return 0;
}

void db_log(char *logfile, char *message, int prio)  // prio 1: ERROR, 2: WARNING, 3: INFO
{
    printf("LOG: %s", message); // Will change to file logging
}