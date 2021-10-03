#include "../include/filehandler.h"

void write_to_db(char * txt,char* path)
{
    FILE * fd;
    fd = fopen(path,"a");
    fprintf(fd,"%s",txt);
    fclose(fd);
}
void write_specific(char * txt,char * path,int start_offset,int end_offset)
{
    FILE * fd;
    fd = fopen(path,"r+");
    if(fd)
    {
        int amount_to_write = end_offset-start_offset;

        printf("Start offset: %d\n",start_offset);
        printf("End offset: %d\n",end_offset);
        
        char temp_file[10000];
        char new_file[10000];
        
        
        read_from_db(path,temp_file,'!','*');
        read_specific(path,new_file,0,start_offset);
        read_specific(path,new_file,end_offset,strlen(temp_file)-1);
        // for(int i = 0; i<strlen(new_file)-1;i++)
        // {
        //     new_file[i] = new_file[i+1];
        // }
        printf("\nNew file:\n%s",new_file);
        fclose(fd);
        fd = fopen(path,"w");
        fputs(new_file,fd);
        fclose(fd);
    }

}
void read_from_db(char*path, char* content_to_read, char start_of_string, char end_of_string)
{
    FILE * fd;
    fd = fopen(path,"r");
    if(fd)
    {
        if(start_of_string != '!' && end_of_string != '*')
        {
            char c  = getc(fd);
            while(!feof(fd))
            {
                if(c == start_of_string)
                {
                    //printf("found start of string\n");
                    size_t content_sz = strlen(content_to_read);
                    c = getc(fd);
                    while( c != end_of_string)
                    {
                        content_to_read[content_sz] = c;
                        content_sz++;
                        //printf("append:%c\n",c);
                        c = getc(fd);
                    }
                    content_to_read[content_sz] = '\n';
                    //printf("found end of string\n");
                }
                else//skip lines
                {
                    c = getc(fd);
                }
            }
        }
        else //läs hela filen
        {
            //printf("Reading whole file....");
            char c;
            size_t content_sz = strlen(content_to_read);
            c = getc(fd);
            while(!feof(fd))
            {
                content_to_read[content_sz] = c;
                content_sz++;
                //printf("append:%c\n",c);
                c = getc(fd);
            }
        }
        fclose(fd);
    }
    else
    {
        printf("file not found!\n");
    }
}
void read_specific(char * path,char * content_to_read,int start_offset,int end_offset)
{
    FILE * fd;
    fd = fopen(path,"r");
    if(fd && end_offset > start_offset)
    {
        int amount_to_read = end_offset - start_offset;
        fseek(fd,start_offset,SEEK_SET);
        printf("Start offset: %d",start_offset);
        printf("End offset: %d",end_offset);
        size_t content_sz = strlen(content_to_read);
        char c = fgetc(fd);
        while(ftell(fd)<=end_offset)
        {
            if(c!='\0')
            {
                content_to_read[content_sz] = c;
                content_sz++;
            }
            c = fgetc(fd);
        }
        content_to_read[content_sz] = '\n';
    }
}
