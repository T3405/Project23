#include "client_cmd.h"
#include "commands.h"
#include <stdio.h>
#include <stdlib.h>

int cmd_read_code(int fd){
    int n = 0;
    read(fd,&n,sizeof(int));
      if(n == CMD_SERVER_OFFLINE){
        printf("The server is offline!\n");
        exit(0);
      }
    return n;
}