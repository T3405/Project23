#include <fcntl.h>
#include <stdio.h>
#include <string.h>


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




int main() {

    //
    signal(SIGINT, alertHandler); // Primo ctrl +c
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);






    // Clear screen
    clear_screen();


    //printf("Creating board\n");



    while (status){
        clear_screen();
        int row = 5;
        int column = 5;
        for (int i = 0; i < row; ++i){
            for (int j = 0; j < column; ++j){
                printf("| ");
            }
            printf("|\n");
        }
        //Todo read matrix

        long n = 0;
        char buffer[BUFSIZ];

        while (n == 0 || n == -1){
            n = read(STDIN_FILENO,buffer,BUFSIZ);
            if(!status){
                break;
            }
            if(n > 0){
                char command[n];
                memcpy(command,buffer,n);

                int x = atoi(command);
                if(x <= 0 || x > column){
                    printf("Please input a valid number!\n");
                    sleep(1);
                }
                //TODO Send column to server
            }
            //Handle command
        }
        printf("n : %ld\n",n);

        //exit(1);


    }
    printf("exit!\n");

    //
    //    unsigned short row = w.ws_row-1;
    //    for (int i = 0; i < row; ++i){
    //            for (int j = 0; j < i; ++j) {
    //                if(i == 0 || i == w.ws_col-1){
    //                    printf("-");
    //                }else{
    //                    if(j == 0 || j == w.ws_col-1){
    //                        printf("|");
    //                    }else{
    //                        printf(" ");
    //                    }
    //                }
    //            }
    //    }
    //    return 0;  // make sure your main returns int
}
