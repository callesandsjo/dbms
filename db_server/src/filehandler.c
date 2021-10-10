#include "../include/filehandler.h"

void write_to_db(char * txt,char* path)
{
    FILE * fd;
    fd = fopen(path,"a");

    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_start = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len = 0;
    fcntl(fileno(fd), F_SETLKW, &lock);

    fprintf(fd,"%s",txt);

    lock.l_type = F_UNLCK;
    lock.l_start = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len = 0;
    fcntl(fileno(fd), F_SETLK, &lock);

    fclose(fd);
}
void write_specific(char * txt,char * path,int start_offset,int end_offset)
{
    long int size = get_file_size(path);
    if(size > -1 && size >= end_offset)
    {
        //int amount_to_write = end_offset-start_offset;

        //printf("Start offset: %d\n",start_offset);
        //printf("End offset: %d\n",end_offset);
        FILE * fd;
        char *temp_file = (char*)malloc(size+256);
        temp_file[0] = 0;
        char *new_file = (char*)malloc(size+256);
        new_file[0]=0;

        //printf("Reading to temp_file...\n");
        read_from_db(path,temp_file,'!','*');
        //printf("Reading to temp_file... done\n");

        //printf("Reading to new_file... start \n");
        read_specific(path,new_file,0,start_offset);
        //printf("Reading to new_file...start done\n");
        int test = strlen(temp_file);
        //printf("Reading to new_file... end %d\n",test);
        read_specific(path,new_file,end_offset,strlen(temp_file));
        //printf("Reading to new_file... end done\n");
        
        //printf("\nNew file:\n%s",new_file);
        fd = fopen(path,"w");
        
        struct flock lock;
        lock.l_type = F_WRLCK;
        lock.l_start = 0;
        lock.l_whence = SEEK_SET;
        lock.l_len = 0;
        fcntl(fileno(fd), F_SETLKW, &lock);

        fputs(new_file,fd);

        lock.l_type = F_UNLCK;
        lock.l_start = 0;
        lock.l_whence = SEEK_SET;
        lock.l_len = 0;
        fcntl(fileno(fd), F_SETLK, &lock);

        fclose(fd);
        
        memset(temp_file,0,strlen(temp_file));
        free(temp_file);
        temp_file = NULL;
        memset(new_file,0,strlen(new_file));
        free(new_file);
        new_file = NULL;
    }

}
void read_from_db(char*path, char* content_to_read, char start_of_string, char end_of_string)
{
    FILE * fd;
    fd = fopen(path,"r");
    if(fd)
    {
        struct flock lock;
        lock.l_type = F_RDLCK;
        lock.l_start = 0;
        lock.l_whence = SEEK_SET;
        lock.l_len = 0;
        fcntl(fileno(fd), F_SETLKW, &lock);

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
                    content_sz++;
                    content_to_read[content_sz] = '\0';
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
            content_to_read[content_sz] = '\0';
        }
        
        lock.l_type = F_UNLCK;
        lock.l_start = 0;
        lock.l_whence = SEEK_SET;
        lock.l_len = 0;
        fcntl(fileno(fd), F_SETLK, &lock);
        fclose(fd);
    }
    else
    {
        //printf("file not found!\n");
    }
}
void read_specific(char * path,char * content_to_read,int start_offset,int end_offset)
{
    long int file_sz = get_file_size(path);
    if(start_offset > file_sz || end_offset > file_sz)
    {
        //printf("start:%d, end:%d while file:%ld\n",start_offset,end_offset,file_sz);
    }
    FILE * fd;
    fd = fopen(path,"r");
    if(fd && (end_offset > start_offset))
    {
        struct flock lock;
        lock.l_type = F_RDLCK;
        lock.l_start = 0;
        lock.l_whence = SEEK_SET;
        lock.l_len = 0;
        fcntl(fileno(fd), F_SETLKW, &lock);

        //printf("Start offset: %d",start_offset);
        //printf("End offset: %d",end_offset);
        //int amount_to_read = end_offset - start_offset;
        fseek(fd,start_offset,SEEK_SET);
        size_t content_sz = strlen(content_to_read);
        char c = fgetc(fd);
        while(ftell(fd)<end_offset)
        {
            if(c!='\0')
            {
                content_to_read[content_sz] = c;
                content_sz++;
            }
            c = fgetc(fd);
        }
        content_to_read[content_sz] = '\n';
        content_sz++;
        content_to_read[content_sz] = '\0';

        lock.l_type = F_UNLCK;
        lock.l_start = 0;
        lock.l_whence = SEEK_SET;
        lock.l_len = 0;
        fcntl(fileno(fd), F_SETLK, &lock);

        fclose(fd);
    }
    else if(fd)
    {
        fclose(fd);
    }
}
void read_column(char * path ,char*content_to_read,char* delims,bool*columns,int nr_of_columns,char mode)
{
    char delimiters[256] = "\t\n ";
    strcat(delimiters,delims); 
    if(mode == 'F'||mode =='f') // path==a path -> read from file
    {
        FILE * fd;
        fd = fopen(path,"r");
        if(fd)
        {
            fclose(fd);
        }

    }
    else if(mode == 'A'|| mode =='a')//path == array
    {
        size_t content_sz = strlen(content_to_read);
        int path_sz = strlen(path);
        for(int i = 0;i<path_sz;i++)
        {
            if(!strchr(delimiters,path[i]))
            {
                for(int j = 0; j<nr_of_columns;j++)
                {
                    if(columns[j]) // add column
                    {
                        //i++;
                        while(!strchr(delimiters,path[i]))//adding value
                        {
                            
                            content_to_read[content_sz] = path[i];
                            content_sz++;
                            i++;
                        }
                        content_to_read[content_sz] = '\t';
                        content_sz++;
                        i++;//skip delimiter
                    }
                    else //skip column
                    {
                        while(!strchr(delimiters,path[i]))
                        {
                            i++;
                        }
                        i++;//skip delimiter
                    }
                }
                content_to_read[content_sz] = '\n';
                content_sz++;
            }
        }
        content_to_read[content_sz] = '\0';
    }
    memset(delimiters,0,sizeof(delimiters));
}
long int get_file_size(char*path)
{
    FILE *fd;
    fd = fopen(path,"r");
    long int file_sz = -1;
    if(fd)
    {
        struct flock lock;
        lock.l_type = F_RDLCK;
        lock.l_start = 0;
        lock.l_whence = SEEK_SET;
        lock.l_len = 0;
        fcntl(fileno(fd), F_SETLKW, &lock);

        fseek(fd,0,SEEK_END);
        file_sz = ftell(fd);
        fseek(fd,0,SEEK_SET);

        lock.l_type = F_UNLCK;
        lock.l_start = 0;
        lock.l_whence = SEEK_SET;
        lock.l_len = 0;
        fcntl(fileno(fd), F_SETLK, &lock);

        fclose(fd);
    }
    return file_sz;
}