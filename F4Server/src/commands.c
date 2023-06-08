#include "commands.h"
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>


//client 0 cmd : CMD_SET_SYMBOL : char
void cmd_send(struct client_info client, int cmd, void* msg){

    write(client.fifo_fd,&cmd,sizeof(cmd));
    long size = cmd / 100;
    //printf("pid : %d, code : %d , size : %ld , fd : %d\n",client.pid,cmd,size,client.fifo_fd);
    if(cmd != 0){
        write(client.fifo_fd,msg, size);
    }
}


void cmd_broadcast(struct client_info clients[], int cmd, void* msg){
    for (int i = 0; i < 2; ++i) {
        cmd_send(clients[i], cmd, msg);
    }
}

struct client_info cmd_turn(struct client_info clients[],int turn){
    cmd_send(clients[turn],CMD_TURN,NULL);
    return clients[turn];
}



int is_alive(pid_t pid){
    if(kill(pid,0) == -1){
        if(errno == ESRCH){
            return 0;
        }
    }
    return 1;
}

int path_size(int id,char* path){
    int n = 0;
    while(id > 0){
        id = id/10;
        n++;
    }
    return n + strlen(path);
}

int cmd_mkfifo(int pid, char* path,int mode){
    char buffer[path_size(pid,path)];
    sprintf(buffer,"%s%d",path,pid);
    mkfifo(buffer, S_IRUSR | S_IWUSR);
    return open(buffer,mode);
}
int cmd_rmfifo(int pid,char* path,int fd){
    char buffer[path_size(pid,path)];
    sprintf(buffer,"%s%d",path,pid);
    close(fd);
    unlink(buffer);
    printf("removing path : %s\n",buffer);
}