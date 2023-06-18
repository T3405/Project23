#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/signal.h>
#include <unistd.h>


#include "ui.h"
#include "f4logic.h"
#include "ioutils.h"
#include "errExit.h"

char active = 1;
char quit = 0;

void signal_exit(int signal){
    printf("Max game reached please try again later\n");
    exit(0);
}

//Close signal function
void signal_close(int signal) {
    active = 0;
    quit = 1;
}

//Confirmation close signal function
void signal_alert(int sig) {
    printf("Are you sure you want to exit, if yes press CTRL + C\n");
    signal(SIGINT, signal_close); // Primo ctrl +c
}

int main(int argc, char *argv[]) {

    signal(SIGUSR2, signal_exit);

    if (argc <= 1) {
        printf("Usage : %s <nickname> (*)\n", argv[0]);
        return 1;
    }
    if (strlen(argv[1]) >= 50) {
        printf("The nickname max length is 49!\n");
        return 1;
    }

    struct client_info clientInfo;
    clientInfo.pid = getpid();

    memcpy(clientInfo.name, argv[1], strlen(argv[1]));
    clientInfo.name[strlen(argv[1])] = '\0';
    // Set check if mode is single player
    clientInfo.mode = '\0';
    if (argc > 2) {
        if (argv[2][0] == '*') {
            clientInfo.mode = argv[2][0];
        }
    }

    // Open default path for fifo
    int fd = open(DEFAULT_PATH, O_WRONLY);
    if (access(DEFAULT_PATH, W_OK) != 0) {
        printf("Please start the server first!\n");
        exit(0);
    }

    // Write to FIFO and connect to client
    write(fd, &clientInfo, sizeof(struct client_info));
    signal(SIGINT, signal_alert);
    signal(SIGUSR1, signal_close);
    signal(SIGHUP, signal_close);
    signal(SIGTSTP, signal_close);

    // Wait for the FIFO file to be created at /tmp/f4/<pid>
    char path[1024];
    sprintf(path, "%s%d", DEFAULT_CLIENTS_DIR, clientInfo.pid);
    printf("Waiting for another client to connect to the server\n");
    while (access(path, F_OK) != 0) {
        if (!active) {
            printf("Server offline\n");
            return 0;
        }
    }
    // Reset errno
    errno = 0;

    // Open the input_fifo
    int input_fd = open(path, O_RDONLY);

    // Wait for symbolInfo
    int code;
    // Read its own symbolInfo
    struct symbol_info symbolInfo;
    code = cmd_read_code(input_fd);
    read(input_fd, &symbolInfo, sizeof(symbolInfo));
    printf("Symbol :  %d,char %c\n", code, symbolInfo.own);
    printf("Enemy Symbol :  %d,char %c\n", code, symbolInfo.enemy);
    printf("Enemy Name : %s\n", symbolInfo.enemy_name);

    // Read gm_info (row,column,key_t shared_mem)
    struct game_info gm_info;
    code = cmd_read_code(input_fd);
    read(input_fd, &gm_info, sizeof(gm_info));
    printf("Game number %d\n", gm_info.id);

    // Attach shared-memory
    int mem_id = shmget(ftok(FTOK_SMH, gm_info.id),
                        gm_info.column * gm_info.row * sizeof(pid_t), 0666);
    if (mem_id == -1) {
        errExit("error shared_memory");
    }
    pid_t *board = shmat(mem_id, NULL, O_RDONLY);

    // Open semaphore
    int sem_id = semget(ftok(FTOK_SEM, gm_info.id), 2, S_IRUSR | S_IWUSR);
    if (sem_id == -1) {
        errExit("error semaphore");
    }

    // Open msg_qq
    int output_qq = msgget(ftok(FTOK_MSG, gm_info.id), 0666);
    if (output_qq == -1) {
        errExit("error message queue");
    }

    // Set the both fd with a O_NON_BLOCK
    fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL) | O_NONBLOCK);
    fcntl(input_fd, F_SETFL, fcntl(input_fd, F_GETFL) | O_NONBLOCK);

    while (active) {
        // Read the start of the fifo
        print_board(board, sem_id, gm_info, symbolInfo);
        code = cmd_read_code(input_fd);
        switch (code) {
            default:
                break;
            case CMD_TURN: {
                printf("It's your turn\n");
                break;
            }
            case CMD_INPUT_ERROR: {
                int error_code = 0;
                while ((read(input_fd, &error_code, sizeof(int)) <= 0) && active);
                switch (error_code) {
                    case -2: {
                        printf("Input out of range try again\n");
                    }
                        break;
                    case -3: {
                        printf("The column is full!\n");
                    }
                        break;
                    case 0: {
                        printf("It's not your turn\n");
                    }
                }
            }
                break;
            case CMD_WINNER: {
                char winner = 0;
                while ((read(input_fd, &winner, sizeof(char)) <= 0) && active);
                if (winner == symbolInfo.own) {
                    printf("You are the winner!\n");
                } else if (winner == symbolInfo.enemy) {
                    printf("You lost!\n");
                } else if (winner == '\0') {
                    printf("Draw!\n");
                }
                active = 0;
                break;
            }
        }
        char input_string[20];
        // Read STDIN and send to the msg_qq
        ssize_t input_read;
        if ((input_read = read(STDIN_FILENO, input_string, 20)) > 0) {
            if (input_string[0] != '\n') {
                if(check_string_num(input_string, input_read)) {
                    struct client_msg msg;
                    msg.mtype = 1;
                    msg.pid = getpid();
                    msg.move = abs(atoi(input_string));
                    msgsnd(output_qq, &msg, sizeof(msg) - sizeof(long), 0);
                }else{
                    printf("Input is not a number\n");
                }
            }
        }
    }
    shmdt(board);

    //Reset the signals
    signal(SIGINT, SIG_DFL);
    signal(SIGHUP, SIG_DFL);
    signal(SIGTSTP, SIG_DFL);

    //Check if you quit using the signals
    if (quit) {
        struct client_msg msg;
        msg.mtype = 1;
        msg.pid = getpid();
        //-1 = Forfeit
        msg.move = -1;
        msgsnd(output_qq, &msg, sizeof(msg) - sizeof(long), 0);
    } else {
        printf("Do you want to replay the game?\nY/N\n");
        char selection;
        while (read(STDIN_FILENO, &selection, sizeof(char)) <= 0);
        if (selection == 'Y' || selection == 'y') {
            execv(argv[0], argv);
        }
    }
    printf("Closing!\n");
    return 0;
}

/************************************
 *Matricola VR473680,VR443698
 *Nome e cognome Alex Zanetti,Federico Rossato
 *Data di realizzazione 28 / 4 / 2023
 *************************************/
