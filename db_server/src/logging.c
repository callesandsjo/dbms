#include "../include/logging.h"


int setup_logging(char *logfile){
    return 0;
}

void db_log(char *logfile, char *message, int prio)  // prio 1: ERROR, 2: WARNING, 3: INFO
{
    if(strlen(logfile) <1 )
    {
        printf("Logging to syslog\n");
        syslog(prio,"%s",message);
    }
    else
    {
        printf("Logging to:'%s'\n",logfile);
        write_to_db(message,logfile);
    }
}
void timestamp(char * buffer) // Following W3C extended format: "YYYY-MM-DD:HH:MM:SS"
{
    time_t utc;
    struct tm *tajm; 
    char test[256];
    
    time(&utc); 
    tajm = gmtime(&utc);

    strftime(test,sizeof(test), "[%F:%T %Z]",tajm);
    strcat(buffer,test);
    //strcpy(buffer,test);

}