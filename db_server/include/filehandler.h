#ifndef FILEHANDLER_H
#define FILEHANDLER_H
#include "../lib/request.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <regex.h>

#define TABLE_DB_PATH "../database/tables.txt"
#define RECORD_DB_PATH "../database/records."

void write_to_db(char * txt,char*path);
void write_specific(char * txt,char * path,int start_offset,int end_offset);
void read_from_db(char*path,char* content_to_read, char start_of_string,char end_of_string);
void read_specific(char * path,char * content_to_read,int start_offset,int end_offset);
bool find_table(char*table);

#endif