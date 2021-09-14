// Main
#include "../include/network.h"
#include "../include/cmdoptions.h"
#include <stdlib.h>
#include <stdio.h>

#define PORT 8080

int main(int argc, char *argv[])
{

    if (handle_options(argc, argv) == -1)
        exit(1);

    int main_socket = create_socket(PORT);
    printf("socket created");
    
    return 0;
}