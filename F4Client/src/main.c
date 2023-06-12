#include <fcntl.h>

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include <sys/signal.h>
#include <sys/shm.h>
#include <unistd.h>

#include "ui.h"

#include "f4logic.h"
#include "commands.h"
#include "client_cmd.h"

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
    if (access(DEFAULT_PATH, W_OK) != 0) {
        printf("Please start the server first!\n");
        exit(0);
    }
    struct client_info clientInfo;
    //Set pid
    clientInfo.pid = getpid();
    //Set check if mode is single player
    clientInfo.mode = '\0';
    if(argc > 1){
        if(argv[1][0] == '*'){
            clientInfo.mode = argv[1][0];
             //modalita' single player
        }
    }
    //Write to FIFO and connect to client
    write(fd,&clientInfo, sizeof(struct client_info));

    char path[1024];
    sprintf(path,"%s%d",DEFAULT_DIR,clientInfo.pid);
    while (access(path, F_OK) != 0) {
        //Wait till the file exists
    }
    errno = 0;
    printf("path : %s\n",path);



    int input_fd = open(path,O_RDONLY);
    //Wait for symbol

    char symbol;
    int code;
    code = cmd_read_code(input_fd);
    read(input_fd,&symbol, sizeof(char));
    printf("code %d,char %c\n",code,symbol);


    key_t output_qq_id;
    code = cmd_read_code(input_fd);
    read(input_fd, &output_qq_id, sizeof (output_qq_id));
    printf("code %d ,key %d\n", code, output_qq_id);

    struct shared_mem_info shared_mem_info;
    code = cmd_read_code(input_fd);
    read(input_fd,&shared_mem_info, sizeof(shared_mem_info));
    printf("shared_key %d\n",shared_mem_info.key);


    int mem_id = shmget(shared_mem_info.key, shared_mem_info.column * shared_mem_info.row * sizeof(pid_t), 0666);
    pid_t* board = shmat(mem_id, NULL, O_RDONLY);

    int output_qq = msgget(output_qq_id,0666);


    //Set the fd with a O_NON_BLOCK
    fcntl(STDIN_FILENO,F_SETFL, fcntl(STDIN_FILENO,F_GETFL)|O_NONBLOCK);
    fcntl(input_fd,F_SETFL, fcntl(input_fd,F_GETFL)|O_NONBLOCK);

    int row = shared_mem_info.row;
    int column = shared_mem_info.column;
    char turn = 0;
    while (status){
        code = cmd_read_code(input_fd);
            switch (code) {
                case 0:
                    break;
                case CMD_UPDATE: {
                    for (int i = 0; i < row; ++i) {
                        for (int j = 0; j < column; ++j) {
                            printf("|%d", GET_M(board, row, i, j));
                        }
                        printf("|\n");
                    }
                    break;
                }
                case CMD_TURN: {
                    turn = 1;
                    printf("It's your turn\n");
                    break;
                }
                case CMD_INPUT_ERROR: {
                    read(input_fd, &code, sizeof(int));
                    if (code == 0) {
                        //Wrong input input
                        turn = 0;
                    } else if (code == 1) {
                        //Print not your turn
                    }
                }
                    break;
                case CMD_WINNER: {
                    char winner;
                    read(input_fd,&winner, sizeof(char));
                    if(winner == symbol) {
                        //You win
                    }else {
                        //You lose
                    }
                    break;
                }
            }
            char input_char[20];
            if(read(STDIN_FILENO,input_char, 20) > 0){
                printf("test\n");
                struct client_msg msg;
                msg.mtype = 1;
                msg.pid = getpid();
                msg.move = atoi(input_char);
                msgsnd(output_qq,&msg, sizeof(msg)- sizeof(long),0);
            }


    }
    printf("exit!\n");

    return 0;
}
