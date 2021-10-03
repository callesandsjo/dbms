#ifndef TABLEHANDLER_H
#define TABLEHANDLER_H
#include "../include/filehandler.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <regex.h>

bool create_table(request_t * req);
void list_tables(char * tables);
void list_schemas(char * schemas,char* table_name);
void drop_table(char * table_name);
void insert_record(request_t * req);
void select_record(request_t * req,char*records,int nr );
bool find_table(char*table);
bool spec_check(char * schemas,column_t * variable,int nr_of);
#endif