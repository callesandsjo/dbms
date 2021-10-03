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
                strcat(to_write, temp->name);
                strcat(to_write,"\tINT,\n");
            }
            else    //lägger in VARCHAR typ i tables
            {
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
void list_schemas(char * schemas,char *table_name) //fungerar konstigt, men bättre än innan 
{
    // char so_string = '['
    // char eo_string = ';'
    // read_from_db(TABLE_DB_PATH,tables,so_string,eo_string);
        regex_t treg;
        regmatch_t  match[1];

        char  pattern_for_tables[4096] = "(";
        strcat(pattern_for_tables,table_name);
        strcat(pattern_for_tables,"\\]\n.+[-A-Za-z\t,\n]*\n)"); // letar efter "('tablename'\]\n.+)"
        char tables[4096];

        read_from_db(TABLE_DB_PATH,tables,'[',';');
        printf("%s\n",tables);

        regcomp(&treg,pattern_for_tables,REG_EXTENDED|REG_NEWLINE);//sätter pattern
        if(regexec(&treg,tables, 1, match,1) == 0)//kollar efter table 
        {
            read_specific(TABLE_DB_PATH,schemas,match[0].rm_so+strlen(table_name)+4,match[0].rm_eo);
            //verkar som att offseten beror på antalet tables?
        }
        else
        {
            printf("No match\n");
        }
        regfree(&treg);
}
void drop_table(char * table_name)//funkar ej riktigt
{
    if(find_table(table_name))
    {
        regex_t treg;
        regmatch_t  match[1];
        char  pattern_for_tables[4096] = "(";
        strcat(pattern_for_tables,table_name);
        strcat(pattern_for_tables,"\\]\n.+\n)");
        char tables[4096];

        read_from_db(TABLE_DB_PATH,tables,'[',';');
        //printf("%s\n",tables);

        regcomp(&treg,pattern_for_tables,REG_EXTENDED|REG_NEWLINE);//sätter pattern

        if(regexec(&treg,tables, 1, match,1) == 0)//kollar efter table 
        {
            write_specific("",TABLE_DB_PATH,match[0].rm_so,match[0].rm_eo);
        }
        else
        {
            printf("Error: Table not found\n");
        }
        regfree(&treg);
    }
}
void insert_record(request_t * req)
{
    print_request(req);
    if(find_table(req->table_name)) // här behövs list schemas ehehheheeheh
    {
        //behöver checka ifall det finns 'support för values'
        char to_write[256] ="[";
        strcat(to_write,req->table_name);
        strcat(to_write,"](");
        column_t * temp = req->columns;
        while(temp)
        {
            if(temp->data_type == DT_INT)
            {
                char int_to_char = temp->int_val +'0';
                strcat(to_write,(char*)&int_to_char); // nåt e sus med denna konvertering
            }
            else
            {
                strcat(to_write,temp->char_val);
            }
            strcat(to_write,", ");
            temp = temp->next;
        }
        strcat(to_write,")\n");
        write_to_db(to_write,RECORD_DB_PATH);
    }

}
void select_record(request_t * req,char*records,int nr) // funkar ej
{
    if(find_table(req->table_name))
    {
        regex_t treg;
        regmatch_t  match[nr];
        char record[4096] ="";
        
        char  pattern_for_record[4096] = "([A-Za-z]+";
        strcat(pattern_for_record,req->table_name);
        strcat(pattern_for_record,"\\].+");


        //printf("Reading records\n");
        read_from_db(RECORD_DB_PATH,record,'[','\n');
        //printf("Reading records... done\n");
        printf("%s\n",record);

        regcomp(&treg,pattern_for_record,REG_EXTENDED|REG_NEWLINE);//sätter pattern
        // if(regexec(&treg,record, nr, match,0) == 0)//kollar efter records 
        // {
        //     for(int i = 0;i<nr;i++)
        //     {
        //         read_specific(TABLE_DB_PATH,records,match[i].rm_so,match[i].rm_eo);
        //     }
        // }
         regfree(&treg);
    }
}
bool find_table(char*table)
{
    bool table_found = false;
    regex_t treg;
    regmatch_t  match;
    char tables[1024];
    list_tables(tables);
    regcomp(&treg,table,REG_NOSUB);//sätter pattern

    if(regexec(&treg,tables, 1, &match,0) == 0)//kollar efter table namnet
    {
        table_found = true;
    }
    regfree(&treg);
    return table_found;
}