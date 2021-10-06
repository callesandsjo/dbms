#include "../include/tablehandler.h"

bool create_table(request_t * req)  //create table
{
    bool exist = find_table(req->table_name);
    //check if tablename already exists
    if(!exist)
    {
        char to_write[256] = "[";
        strcat(to_write,req->table_name);
        strcat(to_write,"]\n");
        column_t *temp = req->columns;
        while(temp)
        {
            strcat(to_write, "-");
            if((int)temp->data_type == DT_INT) //lägger in INT typ i tables
            { 
                if(temp->is_primary_key)
                {
                    strcat(to_write,"(P)");
                    strcat(to_write,"\t");
                }
                strcat(to_write, temp->name);
                strcat(to_write,"\tINT,\n");
            }
            else if(temp->data_type == DT_VARCHAR)   //lägger in VARCHAR typ i tables
            {
                if(temp->is_primary_key)
                {
                    strcat(to_write,"(P)");
                    strcat(to_write,"\t");
                }
                strcat(to_write, temp->name);
                strcat(to_write, "\tVARCHAR(");
                char int_to_char = temp->char_size + '0'; // int +'0' gives the number in ascii table 
                strcat(to_write,(char*)(&int_to_char));
                strcat(to_write,"),\n");
            }
            temp = temp->next;
        }
        strcat(to_write,";\n");
        write_to_db(to_write,TABLE_DB_PATH);
        memset(to_write,0,strlen(to_write));
        /* [ Start of table name
            ] End of table name
            - Start of schema
            , End of schema
            ; End of whole table
            */
    }
    return !exist; // exist = true if it exists => new table not created
}

void list_tables(char * tables)
{
    char so_string = '[';
    char eo_string = ']';
    read_from_db(TABLE_DB_PATH,tables,so_string,eo_string);
}

void list_schemas(char * schemas,char *table_name) //fungerar 
{
    if(find_table(table_name) && get_file_size(TABLE_DB_PATH)>-1)
    {
        regex_t treg;
        regmatch_t  match[1];
        long int file_sz = get_file_size(TABLE_DB_PATH);
        char *tables = (char*)malloc(file_sz+256);
        tables[0] = 0;
        char  * pattern_for_tables = (char*)malloc(256);

        strcpy(pattern_for_tables,"(\\[");
        strcat(pattern_for_tables,table_name);
        strcat(pattern_for_tables,"\\][^;]+;\n)");

        read_from_db(TABLE_DB_PATH,tables,'!','*');
        printf("sz:%d\n File:%s\n",strlen(tables),tables);

        regcomp(&treg,pattern_for_tables,REG_EXTENDED);//sätter pattern
        if(regexec(&treg,tables, 1, match,1) == 0)//kollar efter table 
        {
            read_specific(TABLE_DB_PATH,schemas,match[0].rm_so+strlen(table_name)+3,match[0].rm_eo-3);
        }
        else
        {
            printf("No match\n");
        }
        regfree(&treg);
        memset(tables,0,strlen(tables));
        free(tables);
        memset(pattern_for_tables,0,256);
        free(pattern_for_tables);
    }
}
void drop_table(char * table_name)//funkar men kan inte droppa 2 ggr irad/per session.
{
    long int file_sz = get_file_size(TABLE_DB_PATH);
    if(find_table(table_name) && file_sz>-1)
    {
        regex_t treg;
        regmatch_t  match[1];
        char *tables = (char*)malloc(file_sz+256);
        tables[0] = 0;
        char  * pattern_for_tables = (char*)malloc(256);
        
        strcpy(pattern_for_tables,"(\\[");
        strcat(pattern_for_tables,table_name);
        strcat(pattern_for_tables,"\\][^;]+;\n)");

        read_from_db(TABLE_DB_PATH,tables,'!','*');
        printf("%s\n",tables);

        regcomp(&treg,pattern_for_tables,REG_EXTENDED);//sätter pattern

        if(regexec(&treg,tables, 1, match,1) == 0)//kollar efter table 
        {
            write_specific("",TABLE_DB_PATH,match[0].rm_so,match[0].rm_eo);
            //memset(tables,0,sizeof(tables));
        }
        else
        {
            printf("Error: Table not found\n");
        }

        regfree(&treg);
        memset(tables,0,strlen(tables)+1);
        free(tables);
        memset(pattern_for_tables,0,256);
        free(pattern_for_tables);
    }
}
void insert_record(request_t * req)
{
    //print_request(req);
    if(find_table(req->table_name)) // här behövs list schemas ehehheheeheh
    {
        char * path = (char*)malloc(strlen(RECORD_DB_PATH)+256);
        strcpy(path,RECORD_DB_PATH);
        char to_write[256] = ":";
        char schemas[256]  ="";
        bool check_ok = false;
        bool got_int = false;
        bool got_varchar = false;
        int nr_of_schemas = 0;
        column_t * temp = req->columns;

        strcat(path,req->table_name);
        strcat(path,".txt");
        list_schemas(schemas,req->table_name);

        if(strstr(schemas,"INT"))
        {
            got_int = true;
        }
        if(strstr(schemas,"VARCHAR"))
        {
            got_varchar = true;
        }
        for(int i = 0;i<strlen(schemas);i++)
        {
            if(schemas[i] == '-')
            {
                nr_of_schemas++;
            }
        }
        check_ok = spec_check(schemas,temp,nr_of_schemas);
        while(temp && check_ok)
        {
            strcat(to_write,"(");
            if(temp->data_type == DT_INT && got_int)
            {
                char int_to_char = temp->int_val +'0';
                strcat(to_write,(char*)&int_to_char); // nåt e sus med denna konvertering, funkar dock i create table
                //börjar ge newline från ingenstans!?!?!?!?
            }
            else if(!got_int && got_varchar)
            {
                strcat(to_write,temp->char_val);
            }
            temp = temp->next;
            if(temp)
            {
                strcat(to_write,", ");
            }
            else
            {
                strcat(to_write,");\n");
            }
        }
        if(check_ok)
        {
            write_to_db(to_write,path);
        }
        free(path);
        memset(to_write,0,strlen(to_write));
    }

}
void select_record(request_t * req, char*records, int nr)
{
    if(find_table(req->table_name))
    {
        char path[256] = RECORD_DB_PATH;
        strcat(path,req->table_name);
        strcat(path,".txt");
        read_from_db(path,records,':',';');
    }
}
bool find_table(char*table)
{
    bool table_found = false;
    regex_t treg;
    regmatch_t  match;
    long int file_sz = get_file_size(TABLE_DB_PATH);
    char *tables = (char*)malloc(file_sz+256);
    tables[0] = 0;

    list_tables(tables);
    printf("Tables:\n%s",tables);
    regcomp(&treg,table,REG_NOSUB);//sätter pattern

    if(regexec(&treg,tables, 1, &match,0) == 0)//kollar efter table namnet
    {
        table_found = true;
        //printf("TABLE FOUND!!!!\n");
    }

    regfree(&treg);
    memset(tables,0,strlen(tables));
    free(tables);
    return table_found;
}
bool spec_check(char * schemas,column_t * variable,int nr_of)
{
    bool check = false;
    bool check_int = false;
    bool check_varchar = false;
    column_t * temp = variable;
    int check_nr_of = 0;
    while(temp)
    {
        check_nr_of++;
        if(temp->data_type == DT_INT && strstr(schemas,"INT"))
        {
            check_int = true;
        }
        else if(temp->data_type == DT_VARCHAR && strstr(schemas,(char*)(&temp->char_size + '0')))
        {
            check_varchar = true;
        }
        temp = temp->next;
    }

    if(check_nr_of == nr_of && (check_int||check_varchar))
    {
        check = true;
    }
    return check;
}