#ifndef CMD_OPTIONS_H
#define CMD_OPTIONS_H

int handle_options(int argc, char *argv[]);

void help();

void run_as_daemon();

void choose_port();

void log_setup();

#endif