#include <fcntl.h>
#include <stdio.h>
#include <string.h>

#include <curses.h>


#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/signal.h>
#include <unistd.h>
#include "ui.h"

#include "commands.h"

char status = 1;


//funzione per il secondo segnale d'uscita ctrl+c
void signalHandler(int signal) {
    status = 0;
    //Mandare signal abbandono
}

//funzione per il primo segnale d'uscita ctrl+c
void alertHandler(int sig){
    printf("Sei sicuro di uscire se si ripremi ctrl + c\n");
    signal(SIGINT, signalHandler); // Primo ctrl +c
}






int main(int argc,char* argv[]) {

    signal(SIGINT, alertHandler); // Primo ctrl +c


    int fd = open(DEFAULT_PATH,O_WRONLY);


    struct client_info clientInfo;
    //Set pid
    clientInfo.pid = getpid();
    //Set check if mode is single player
    if(argc > 1){
        if(argv[1] == "*"){
            clientInfo.mode = '*';  //modalita' single player
        }
    }
    clientInfo.message_qq = ftok(".",getpid());

    //Write to FIFO and connect to client
    write(fd,&clientInfo, sizeof(struct client_info));





    initscr();
    int x =0, y=0;
    while (status){
        int row = 5;
        int column = 5;
        for (int i = 0; i < row; ++i){
            for (int j = 0; j < column; ++j){
                printw("| ");
            }
            printw("|\n");
        }
        //Todo read matrix


        int x = getch();
        if(x == 'z'){
            break;
        }
        if(x == 'w'){
            move(y++,x);
        }else if(x == 's'){
            move(y--,x);
        }

        refresh();

        erase();

    }
    endwin();
    printf("exit!\n");

    return 0;
}
