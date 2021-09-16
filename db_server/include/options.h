#ifndef OPTIONS_H
#define OPTIONS_H

int handle_options(int argc, char *argv[], int *port, char **logfile);

void help();

void run_as_daemon();

#endif