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
                char int_to_string[64]; 
                sprintf(int_to_string,"%d",temp->char_size); 
                strcat(to_write,int_to_string);
                strcat(to_write,"),\n");
                memset(int_to_string,0,64);
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
        tables = NULL;
        memset(pattern_for_tables,0,256);
        free(pattern_for_tables);
        pattern_for_tables = NULL;
    }
}
void drop_table(char * table_name)
{
    long int file_sz = get_file_size(TABLE_DB_PATH);
    if(find_table(table_name) && file_sz>-1)
    {
        regex_t treg;
        regmatch_t  match[1];
        char *tables = (char*)malloc(file_sz+256);
        strcpy(tables,"");
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

        memset(tables,0,strlen(tables));
        free(tables);
        tables = NULL;

        memset(pattern_for_tables,0,256);
        free(pattern_for_tables);
        pattern_for_tables = NULL;
    }
}
void insert_record(request_t * req)
{
    //print_request(req);
    if(find_table(req->table_name))
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
            if(strlen(to_write)== 1)
            {
                strcat(to_write,"(");
            }
            if(temp->data_type == DT_INT && got_int)
            {
                char int_to_string[64];
                sprintf(int_to_string,"%d",temp->int_val);
                strcat(to_write,int_to_string); 
                memset(int_to_string,0,64);
            }
            else if(temp->data_type == DT_VARCHAR && got_varchar)
            {
                strcat(to_write,temp->char_val);
            }
            temp = temp->next;
            if(temp)
            {
                strcat(to_write,",");
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
        memset(path,0,strlen(path));
        free(path);
        path = NULL;
        memset(to_write,0,sizeof(to_write));
    }

}
void select_record(request_t * req, char*records, int nr)
{
    char path[256] = RECORD_DB_PATH;
    strcat(path,req->table_name);
    strcat(path,".txt");
    long int file_sz= get_file_size(path);
    if(find_table(req->table_name) && file_sz > 0)
    {
        //if * or specified
        if(req->columns) //specified
        {  
            char* schemas = (char*)malloc(get_file_size(TABLE_DB_PATH));
            strcpy(schemas,"");
            size_t nr_of_schemas = 0;
            column_t * temp = req->columns;
            //list schemas
            list_schemas(schemas,req->table_name);
            //printf("\nSchemas:%s\n",schemas);
            for(int i = 0;i<strlen(schemas);i++)//räknar antalet schemas
            {
                if(schemas[i] == '-')
                { 
                    nr_of_schemas++;
                }
            }
            
            bool schema_nr[nr_of_schemas];
            for(int i = 0;i<nr_of_schemas;i++) //setting all schemas to false
            {
                schema_nr[i] = false; 
            } 

            while(temp) // bestämmer vilka schemas som skall vara med
            {
                size_t char_index = 0;
                //printf("Schema name selected:%s\n",temp->name);
                //printf("%d:%c\n",schemas[char_index]);
                for(int i = 0; i < nr_of_schemas;i++)
                {
                    if(char_index <strlen(schemas))
                    {
                        char schema_name[256];
                        strcpy(schema_name,"");
                        size_t name_sz = 0;
                        char_index++;   //skipping '-'
                        while(schemas[char_index] != '\t')//add until schema type
                        {
                            schema_name[name_sz] = schemas[char_index];
                            name_sz++;
                            char_index++;
                        }
                        printf("\nSchema name:%s\n",schema_name);
                        if(strcmp(temp->name,schema_name) == 0)
                        {
                            schema_nr[i] = true; //true = schema is selected
                            printf("Adding: %s\n",schema_name);
                        }
                        while(schemas[char_index] != '-' && char_index < strlen(schemas))//skip to next schema
                        {
                            char_index++;
                        }
                        memset(schema_name,0,strlen(schema_name));
                    }
                }
                temp = temp->next;
            }
            
            char * temp_records = (char*)malloc(file_sz);
            strcpy(temp_records,"");
            read_from_db(path,temp_records,':',';');
            //printf("temp_records:%s\n",temp_records);
            size_t records_sz = strlen(records);
            for(int i = 0;i<strlen(temp_records);i++)
            {
                if(temp_records[i] == '(')
                {
                    for(int j = 0;j<nr_of_schemas;j++)
                    {
                        i++;
                        if(schema_nr[j])
                        {
                            while(temp_records[i] != ',' && temp_records[i] != ')')
                            {
                                records[records_sz] = temp_records[i];
                                records_sz++;
                                i++;
                            }
                            records[records_sz] = '\t';
                            records_sz++;
                        }
                        else
                        {
                            while(temp_records[i] != ','  && temp_records[i] != ')')
                            {
                                i++;
                            }
                        }
                    }
                    records[records_sz] = '\n';
                    records_sz++;
                }
            }
            
            memset(schemas,0,strlen(schemas));
            free(schemas);
            schemas = NULL;
            
            memset(temp_records,0,strlen(temp_records));
            free(temp_records);
            temp_records = NULL;
        }
        else // SELECT *
        {
            read_from_db(path,records,':',';');
        }
        
        memset(path,0,sizeof(path));
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
    //printf("Tables:\n%s",tables);
    regcomp(&treg,table,REG_NOSUB);//sätter pattern

    if(regexec(&treg,tables, 1, &match,0) == 0)//kollar efter table namnet
    {
        table_found = true;
        //printf("TABLE FOUND!!!!\n");
    }

    regfree(&treg);
    memset(tables,0,strlen(tables));
    free(tables);
    tables = NULL;
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