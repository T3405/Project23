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

char active = 1;
char quit = 0;


//funzione per il secondo segnale d'uscita ctrl+c
void signal_close(int signal) {
    active = 0;
    quit = 1;
}

//funzione per il primo segnale d'uscita ctrl+c
void signal_alert(int sig) {
    printf("Sei sicuro di uscire se si ripremi ctrl + c\n");
    signal(SIGINT, signal_close); // Primo ctrl +c
}


int main(int argc, char *argv[]) {

    signal(SIGINT, signal_alert);

    //Open default path for fifo
    int fd = open(DEFAULT_PATH, O_WRONLY);
    if (access(DEFAULT_PATH, W_OK) != 0) {
        printf("Please start the server first!\n");
        exit(0);
    }

    struct client_info clientInfo;
    //Set pid
    clientInfo.pid = getpid();
    //Set check if mode is single player
    clientInfo.mode = '\0';
    if (argc > 1) {
        if (argv[1][0] == '*') {
            clientInfo.mode = argv[1][0];
            //modalita' single player
        }
    }
    //Write to FIFO and connect to client
    write(fd, &clientInfo, sizeof(struct client_info));

    //Wait for the FIFO file to be created at /tmp/f4/<pid>
    char path[1024];
    sprintf(path, "%s%d", DEFAULT_CLIENTS_DIR, clientInfo.pid);
    while (access(path, F_OK) != 0);
    errno = 0;

    //printf("path : %s\n",path);



    //Open the input_fifo
    int input_fd = open(path, O_RDONLY);
    //Wait for symbol


    //Read its own symbol
    char symbol[2];
    int code;
    code = cmd_read_code(input_fd);
    read(input_fd, &symbol[0], sizeof(char));
    printf("code %d,char %c\n", code, symbol[0]);

    //Read opponent symbol
    code = cmd_read_code(input_fd);
    read(input_fd, &symbol[1], sizeof(char));
    printf("code %d,char %c\n", code, symbol[1]);


    //Read msg_qq key_t
    key_t output_qq_id;
    code = cmd_read_code(input_fd);
    read(input_fd, &output_qq_id, sizeof(output_qq_id));
    printf("code %d ,key %d\n", code, output_qq_id);

    //Read shared_mem_info (row,column,key_t shared_mem)
    struct shared_mem_info shared_mem_info;
    code = cmd_read_code(input_fd);
    read(input_fd, &shared_mem_info, sizeof(shared_mem_info));
    printf("shared_key %d\n", shared_mem_info.key);


    //Attach shared-memory
    int mem_id = shmget(shared_mem_info.key, shared_mem_info.column * shared_mem_info.row * sizeof(pid_t), 0666);
    pid_t *board = shmat(mem_id, NULL, O_RDONLY);

    //Open msg_qq
    int output_qq = msgget(output_qq_id, 0666);


    //Set the both fd with a O_NON_BLOCK
    fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL) | O_NONBLOCK);
    fcntl(input_fd, F_SETFL, fcntl(input_fd, F_GETFL) | O_NONBLOCK);


    unsigned int row = shared_mem_info.row;
    unsigned int column = shared_mem_info.column;

    while (active) {
        //Read the start of the fifo
        code = cmd_read_code(input_fd);
        switch (code) {
            case 0:
                break;
            case CMD_UPDATE: {
                //Print matrix to screen
                for (int i = 0; i < row; ++i) {
                    for (int j = 0; j < column; ++j) {
                        pid_t player_id = GET_M(board, row, i, j);
                        char print_symbol;
                        if (player_id == getpid()) {
                            print_symbol = symbol[0];
                        } else if (player_id == 0) {
                            print_symbol = ' ';
                        } else {
                            print_symbol = symbol[1];
                        }
                        printf("|%c", print_symbol);
                    }
                    printf("|\n");
                }
                break;
            }
            case CMD_TURN: {
                printf("It's your turn\n");
                break;
            }
            case CMD_INPUT_ERROR: {
                int error_code;
                read(input_fd, &error_code, sizeof(int));
                printf("error : %d\n", error_code);
                if (error_code == 1) {
                    //Wrong input input
                    printf("Wrong input please try again\n");
                } else if (error_code == 2) {
                    printf("It's not your turn\n");
                    //Print not your turn
                }
            }
                break;
            case CMD_WINNER: {
                char winner = 0;
                read(input_fd, &winner, sizeof(char));
                if (winner == symbol[0]) {
                    printf("You are the winner!\n");
                    //You win
                } else {
                    printf("You lost!\n");
                }
                active = 0;
                break;
            }
        }
        char input_char[20];
        //Read STDIN and send to the msg_qq
        if (read(STDIN_FILENO, input_char, 20) > 0) {
            struct client_msg msg;
            msg.mtype = 1;
            msg.pid = getpid();
            //-1 because the board start at 1
            msg.move = abs(atoi(input_char) - 1);


            msgsnd(output_qq, &msg, sizeof(msg) - sizeof(long), 0);
        }
    }
    if(quit){
        struct client_msg msg;
        msg.mtype = 1;
        msg.pid = getpid();
        //-1 = abandon
        msg.move = -1;
        msgsnd(output_qq,&msg, sizeof(msg)- sizeof(long),0);
    }
    printf("exit!\n");
    return 0;
}
