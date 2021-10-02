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
        char temp_file[4096];
        int sz = strlen(temp_file);
        char c;
        //rewind(fd);
        while(fgets(temp_file,256,fd))
        {
            if(ftell(fd) < (long int)start_offset-1)
            {
                fgets(temp_file,start_offset,fd);
            }
            else if(ftell(fd) > (long int)end_offset+1)
            {
                fgets(temp_file,256,fd);
            }
            else
            {
                fseek(fd,end_offset+1,SEEK_SET);
            }
        }
        // while(!feof)
        // {
        //     c = getc(fd);
        //     if(sz < start_offset-1)
        //     {
        //         c = getc(fd);
        //         temp_file[sz] = c;
        //         sz++;
        //     }
        //     else if(sz > end_offset)
        //     {
        //         c = getc(fd);
        //         temp_file[sz] = c;
        //         sz++;
        //     }
        //     else if (strlen(txt) == 0)
        //     {
        //         sz+=end_offset+1;
        //     }
        //     else
        //     {
        //         for(int i = 0;i<strlen(txt);i++)
        //         {
        //           temp_file[sz] = txt[i];  
        //           sz++;
        //         }
        //     }
        // }
        printf("\nNew file:\n%s",temp_file);
        //fattar ej vf den inte läser in filen!?!?!?!?
        fclose(fd);
        fd = fopen(path,"w");
        fputs(temp_file,fd);
    }

}
void read_from_db(char*path, char* content_to_read, char start_of_string, char end_of_string)
{
    FILE * fd;
    fd = fopen(path,"r");
    if(fd)
    {
        char c  = getc(fd);
        while(!feof(fd))
        {
            if(c == start_of_string)
            {
                size_t content_sz = strlen(content_to_read);
                //printf("found start of string\n");
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
                //printf("skipping....\n");
                while(c != '\n')
                {
                    c = getc(fd);
                    //printf("skip:%c\n",c);
                }
            }
            if(c != start_of_string || c== end_of_string)
            {
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
    if(fd)
    {
        int amount_to_read = end_offset - start_offset;
        fseek(fd,start_offset,SEEK_SET);
        printf("Start offset: %d",start_offset);
        printf("End offset: %d",end_offset);
        size_t content_sz = strlen(content_to_read);
        char c;
        for(int i = 0; i <amount_to_read;i++)
        {
            c = getc(fd);
            content_to_read[content_sz] = c;
            content_sz++;
        }
        content_to_read[content_sz] = '\n';
    }
}
