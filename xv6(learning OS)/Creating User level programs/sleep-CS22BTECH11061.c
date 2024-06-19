#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
    //checking correct number of arguments
    if(argc != 2){
        fprintf(2, "Usage: sleep <ticks>\n");
        exit(1);
    }

    //getting the number of ticks
    int ticks = atoi(argv[1]);
    if(ticks < 0){
        fprintf(2, "sleep: invalid number of ticks\n");
        exit(1);
    }
    
    //sleeping for the given number of ticks
    sleep(ticks*10);

    exit(0);
}

