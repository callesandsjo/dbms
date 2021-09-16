#ifndef LOGGING_H
#define LOGGING_H

enum Prio {
    LOG_ERROR = 1,
    LOG_WARNING = 2,
    LOG_INFO = 3
};

int setup_logging(char *logfile);

void db_log(char *logfile, char *message, int prio);  // prio 1: ERROR, 2: WARNING, 3: INFO

#endif