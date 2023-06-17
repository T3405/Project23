#include "serverbot.h"

#include "commands.h"
#include "f4logic.h"
#include "ioutils.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <time.h>

char active = 1;

void signal_stop(int signal) {
    active = 0;
}

void send_random(int output_qq,int random_num){
    struct client_msg msg;
    msg.pid = getpid();
    msg.mtype = 1;
    msg.move = random_num;
    msgsnd(output_qq, &msg, sizeof(msg) - sizeof(long), 0);
}

void f4_bot(int n_game) {
    signal(SIGINT, SIG_IGN);
    signal(SIGUSR1,signal_stop);
    signal(SIGHUP,signal_stop);
    signal(SIGTSTP,signal_stop);

    printf("[%d]Bot starting pid(%d)\n", n_game, getpid());
    //Waiting for server to open fifo
    char path[1024];
    sprintf(path, "%s%d", DEFAULT_CLIENTS_DIR, getpid());
    while (access(path, F_OK) != 0) {
        if (!active) {
            exit(0);
        }
    }
    errno = 0;

    //Open the input_fifo
    int input_fd = open(path, O_RDONLY);

    //Wait for symbol
    int code;
    //Read its own symbol
    struct symbol_info symbol;
    code = cmd_read_code(input_fd);
    read(input_fd, &symbol, sizeof(symbol));

    struct game_info gm_info;
    code = cmd_read_code(input_fd);
    read(input_fd, &gm_info, sizeof(gm_info));


    //Attach shared-memory
    int mem_id = shmget(ftok(FTOK_SMH, gm_info.id), gm_info.column * gm_info.row * sizeof(pid_t), 0666);
    pid_t *board = shmat(mem_id, NULL, O_RDONLY);

    //Open semaphore
    int sem_id = semget(ftok(FTOK_SEM, gm_info.id), 2, S_IRUSR | S_IWUSR);

    //Open msg_qq
    int output_qq = msgget(ftok(FTOK_MSG, gm_info.id), 0666);


    fcntl(input_fd, F_SETFL, fcntl(input_fd, F_GETFL) | O_NONBLOCK);

    unsigned int row = gm_info.row;
    unsigned int column = gm_info.column;
    char is_max[column];

    while (active) {
        //Read the start of the fifo
        if (semaphore_use(sem_id, symbol.pos)) {
            for (int i = 0; i < column; ++i) {
                if (GET_M(board, row, i, row) == 0) {
                    is_max[i] = 0;
                } else {
                    is_max[i] = 1;
                }
            }
        }
        code = cmd_read_code(input_fd);
        switch (code) {
            default:
                break;
            case CMD_TURN: {
                srand(time(NULL));
                int random_num;
                while (active) {
                    random_num = rand() % column;
                    if (!is_max[random_num]) {
                        break;
                    }
                }
                send_random(output_qq,random_num);
                break;
            }
            case CMD_INPUT_ERROR: {
                int error_code = 0;
                while (read(input_fd, &error_code, sizeof(int)) <= 0);
                srand(time(NULL));
                int random_num;
                while (active) {
                    random_num = rand() % column;
                    if (!is_max[random_num]) {
                        break;
                    }
                }
                send_random(output_qq,random_num);
            }
                break;
            case CMD_WINNER: {
                active = 0;
                break;
            }
        }
    }
    //Detaching shared memory
    shmdt(board);
    printf("[%d]Bot closing!\n", gm_info.id);
}

/************************************
*Matricola VR473680,VR443698
*Nome e cognome Alex Zanetti,Federico Rossato
*Data di realizzazione 15 / 06 / 2023
*************************************/