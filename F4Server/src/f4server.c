#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/sem.h>

#include "ioutils.h"
#include "errExit.h"
#include "commands.h"
#include "f4logic.h"


static volatile int active = 1;


void signal_close(int signal) {
    active = 0;
}

//funzione per il primo segnale d'uscita ctrl+c
void signal_alert(int sig) {
    printf("Sei sicuro di uscire se si ripremi ctrl + c\n");
    signal(SIGINT, signal_close); // Primo ctrl +c
    signal(SIGTSTP, signal_close); //Set CTRL+Z
}



int main(int argc, char *argv[]) {

    signal(SIGINT, signal_alert); // Read CTRL+C and stop the program
    signal(SIGTSTP, signal_alert); //Set CTRL+Z

    //Check min argument
    if (argc < 5) {
        printf("Usage: %s <n_row> <n_column> sym_1 sym_2\n", argv[0]);
        return 0;
    }
    //Check -f
    if (argc == 6 && !strcmp(argv[5], "-f")) {
        clean_everything();
    }


    int row = atoi(argv[1]);
    int column = atoi(argv[2]);

    //Check if the size is valid
    if (row < 5) {
        printf("The input row must be >= 5\n");
        return 0;
    }
    if (column < 5) {
        printf("The input column must be <= 5\n");
        return 0;
    }

    //Set player symbols
    char symbols[2];
    if (*argv[3] == *argv[4]) {
        printf("Can't use same symbols\n");
        return 0;
    }
    symbols[0] = *argv[3];
    symbols[1] = *argv[4];





    //Creation default dir
    if (mkdir(DEFAULT_CLIENTS_DIR, 0777) == -1) {
        printf("Can't create dir at %s\n", DEFAULT_CLIENTS_DIR);
        errExit("error");
    }


    //Create fifo for connecting
    if (mkfifo(DEFAULT_PATH, S_IRUSR | S_IWUSR) != 0) {
        printf("There is already an instance of f4server running\n");
        printf("If you want to force start please use -f as last argument\n");
        exit(0);
    }
    int fd_fifo_first_input = open(DEFAULT_PATH, O_RDONLY | O_NONBLOCK);


    //pid_t games[1024];
    int n_game = 0;
    struct client_info clients[2];
    int queue_size = 0;
    printf("[Main]Waiting for clients\n");
    while (1) {
        //Read a client info from the FIFO
        struct client_info buffer;
        ssize_t n = read(fd_fifo_first_input, &buffer, sizeof(struct client_info));
        if (n > 0) {
            printf("[Main]Client connecting with :\n");
            printf("[Main]pid :%d\n", buffer.pid);
            printf("[Main]key id : %d\n", buffer.fifo_fd);
            printf("[Main]mode : %c\n", buffer.mode);

            if (buffer.mode == '*') {
                //TODO Single mode
            } else {
                clients[queue_size] = buffer;
                queue_size++;
                if (queue_size == 2) {
                    queue_size = 0;

                    //Check if client are alive before starting the game
                    if (!is_alive(clients[0].pid)) {
                        printf("[Main]The process %d is not alive\n", clients[0].pid);
                        clients[0] = clients[1];
                        queue_size = 1;
                        continue;
                    }
                    if (!is_alive(clients[1].pid)) {
                        printf("[Main]The process %d is not alive\n", clients[1].pid);
                        queue_size = 1;
                        continue;
                    }


                    //TODO Max games
                    if (fork() == 0) {
                        break;
                    }
                    n_game++;
                }
            }
        }

        //Sopra tutto giusto
        if (active == 0) {
            printf("[Main]Stopping server\n");
            //Close main FIFO
            close(fd_fifo_first_input);
            unlink(DEFAULT_PATH);

            //Wait for every child to terminate
            pid_t child;
            int status;
            while ((child = waitpid(0, &status, 0)) != -1) {
                remove_from_key(child);
                printf("[Main]Child %d has been terminated\n", child);
            }

            //Remove f4 folder
            rmdir(DEFAULT_CLIENTS_DIR);
            //Close fifo
            return 0;
        }
    }

    //Child ---------------------------------------------------------
    printf("[%d]Starting game\n", n_game);

    //Create a fifo for each client in the folder f4
    for (int i = 0; i < 2; ++i) {
        clients[i].fifo_fd = cmd_mkfifo(clients[i].pid, DEFAULT_CLIENTS_DIR, O_WRONLY);
    }
    printf("[%d]Sending Symbols\n", n_game);
    for (int i = 0; i < 2; ++i){
        struct symbol_info info;
        info.pos = i;
        info.own = symbols[i];
        info.enemy = symbols[!i];
        cmd_send(clients[i],CMD_SET_SYMBOLS,&info);
    }


    //Broadcast the key_t for the input msg qq
    key_t key_msg_qq_input = ftok(DEFAULT_PATH, n_game); //key dei client
    //Creation msg qq
    int msg_qq_input = msgget(key_msg_qq_input, 0666 | IPC_CREAT);

    printf("[%d]Sending input queue (key : %d)\n", n_game, key_msg_qq_input);
    cmd_broadcast(clients, CMD_SET_MSG_QQ_ID, &key_msg_qq_input);

    //Calculating the size of the shared memory
    struct game_info shm_mem_inf;
    shm_mem_inf.row = row;
    shm_mem_inf.column = column;

    //Key for the shared memory and the semaphore
    printf("[%d]Key creation\n", n_game);
    shm_mem_inf.key = ftok(".", n_game); //key shared_mem


    printf("[%d]Create shared mem\n", n_game);
    //Create shared memory
    int shm_id =
            shmget(shm_mem_inf.key, row * column * sizeof(pid_t), IPC_CREAT | S_IRUSR | S_IWUSR | S_IROTH);
    if (shm_id == -1) {

    }
    printf("[%d]Attach shared mem (id : %d)\n", n_game, shm_id);
    pid_t *board = (pid_t *) shmat(shm_id, NULL, 0);

    printf("[%d]Create Semaphore\n",n_game);
    int semaphore_id = semaphore_create(shm_mem_inf.key);


    //Fill the array with 0
    printf("[%d]Cleaning array\n", n_game);
    clean_array(board, row, column);

    printf("[%d]Sending shared mem info (key : %d)\n", n_game, shm_mem_inf.key);
    //Broadcast info of shared memory to clients
    cmd_broadcast(clients, CMD_SET_SH_MEM, &shm_mem_inf);

    //Tell the client to read the shared memory
    // legge da memoria condivisa CMD_UPDATE
    printf("[%d]Update board\n", n_game);
    semaphore_set(semaphore_id,1);
    cmd_broadcast(clients, CMD_UPDATE, NULL);


    //Set the turn to the first player
    int turn_num = 0; //turno primo client
    struct client_info player = cmd_turn(clients, turn_num); //giocatore che sta facendo la mossa
    struct client_msg client_mv_buffer;
    unsigned long alive_timer = time(NULL);
    //Main game loop
    while (active) {

        if(alive_timer + 5 <= time(NULL)){
            printf("checking clients\n");
            if(!is_alive(clients[0].pid)) {
                cmd_send(clients[1],CMD_WINNER,&symbols[1]);
                break;
            }else if(!is_alive(clients[1].pid)) {
                cmd_send(clients[0],CMD_WINNER,&symbols[0]);
                break;
            } else{
                alive_timer = time(NULL);
            }
        }
        //Read incoming msg queue non blocking
        if (msgrcv(msg_qq_input, &client_mv_buffer, sizeof(client_mv_buffer) - sizeof(long), 1,
                   MSG_NOERROR | IPC_NOWAIT) > 0) {
            //Check if a player has abandon the game
            if (client_mv_buffer.move == -1) {
                if (client_mv_buffer.pid == player.pid) {
                    cmd_broadcast(clients, CMD_WINNER, &symbols[!turn_num]);
                } else {
                    cmd_broadcast(clients, CMD_WINNER, &symbols[turn_num]);
                }
                break;
            }
            //Check if sender is the player
            if (client_mv_buffer.pid != player.pid) {
                int error = 2;
                cmd_send(clients[!turn_num], CMD_INPUT_ERROR, &error);
                continue;
            }
            //Wait to modify the array
            if(!semaphore_check(semaphore_id)){
                printf("[%d] Waiting for semaphore to unlock\n",n_game);
                continue;
            }
            //Play the move
            pid_t result = f4_play(board, client_mv_buffer.move, client_mv_buffer.pid, column, row);
            if (result == -1) {
                //Wrong input
                int error = 1;
                cmd_send(player, CMD_INPUT_ERROR, &error);
                continue;
            }else if (result == -2){
                //Matrix piena
                char tie_char = '0';
                cmd_broadcast(clients,CMD_WINNER,&tie_char);
            } else if (result == player.pid) {
                //Winner
                cmd_broadcast(clients, CMD_WINNER, &symbols[turn_num]);
                break;
            }
            turn_num = !turn_num;

            //Send update command
            semaphore_set(semaphore_id,1);
            cmd_broadcast(clients, CMD_UPDATE, NULL);
            player = cmd_turn(clients, turn_num);
        }
    }

    //If the server is not active then tell the clients the server is going offline
    if (!active) {
        kill(clients[0].pid,SIGUSR1);
        kill(clients[1].pid,SIGUSR1);
    }


    //Removing shared memory
    shmdt(board);
    shmctl(shm_id, IPC_RMID, NULL);

    //Removing semaphore
    semctl(semaphore_id,0,IPC_RMID);

    //Removing message queue
    msgctl(msg_qq_input,IPC_RMID,NULL);

    //Close all pipes
    for (int i = 0; i < 2; ++i) {
        printf("[%d] Removing file %s%d\n",n_game,DEFAULT_CLIENTS_DIR,clients[i].fifo_fd);
        cmd_rmfifo(clients[i].pid, DEFAULT_CLIENTS_DIR, clients[i].fifo_fd);
    }



    printf("[%d] Game ended\n",n_game);
}


/************************************
*Matricola VR473680
*Nome e cognome Alex Zanetti
*Data di realizzazione 28 / 4 / 2023
*************************************/