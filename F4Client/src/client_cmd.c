#include "client_cmd.h"
#include "commands.h"


int cmd_read_code(int fd){
    int n = 0;
    read(fd,&n,sizeof(int));
    return n;
}