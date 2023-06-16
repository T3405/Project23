#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <errno.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>

#include "commands.h"
#include "errExit.h"
#include "f4logic.h"
#include "ioutils.h"
#include "serverbot.h"

static volatile int active = 1;

void signal_close(int signal) { active = 0; }

// funzione per il primo segnale d'uscita ctrl+c
void signal_alert(int sig) {
    printf("Sei sicuro di uscire se si ripremi ctrl + c\n");
    signal(SIGINT, signal_close);  // Primo ctrl +c
}

int main(int argc, char *argv[]) {

    signal(SIGHUP,signal_close);
    signal(SIGINT, signal_alert);  // Read CTRL+C
    signal(SIGTSTP, signal_close); // Set CTRL+Z

    // Check min argument
    if (argc < 6) {
        printf("Usage: %s <n_row> <n_column> sym_1 sym_2 time_out (-f)\n", argv[0]);
        exit(0);
    }

    int time_out = atoi(argv[5]);
    if (time_out < 4) {
        printf("Timeout can't be lower than 5\n");
        exit(0);
    }

    // Check -f
    if (argc == 7 && !strcmp(argv[6], "-f")) {
        clean_everything();
    }

    int row = atoi(argv[1]);
    int column = atoi(argv[2]);

    // Check if the size is valid
    if (row < 5) {
        printf("The input row must be >= 5\n");
        exit(0);
    }
    if (column < 5) {
        printf("The input column must be >= 5\n");
        exit(0);
    }

    // Set player symbols
    char symbols[2];
    if (*argv[3] == *argv[4]) {
        printf("Can't use same symbols\n");
        return 0;
    }
    symbols[0] = *argv[3];
    symbols[1] = *argv[4];

    // Create fifo for connecting
    if (mkfifo(DEFAULT_PATH, S_IRUSR | S_IWUSR) != 0) {
        printf("There is already an instance of f4server running\n");
        printf("If you want to force start please use -f as last argument\n");
        exit(0);
    }
    int fd_fifo_first_input = open(DEFAULT_PATH, O_RDONLY | O_NONBLOCK);

    // Creation default dir
    if (mkdir(DEFAULT_CLIENTS_DIR, 0777) == -1) {
        printf("Can't create dir at %s\n", DEFAULT_CLIENTS_DIR);
        errExit("error");
    }


    // pid_t games[1024];
    int n_game = 0;
    struct client_info clients[2];
    int queue_size = 0;
    printf("[Main]Waiting for clients\n");
    while (1) {
        // Read a client info from the FIFO
        struct client_info buffer;
        ssize_t n = read(fd_fifo_first_input, &buffer, sizeof(struct client_info));
        if (n > 0) {
            printf("[Main]Client connecting : pid : %d , mode : \'%c\' , name : %s\n", buffer.pid, buffer.mode,
                   buffer.name);


            if (buffer.mode == '*') {
                int bot_id = fork();
                if (bot_id == 0) {
                    //Bot child
                    f4_bot(n_game);
                    exit(0);
                }
                if (fork() == 0) {
                    //Bot creation
                    clients[0] = buffer;
                    struct client_info bot;
                    bot.pid = bot_id;
                    memcpy(bot.name, "Bot", 3);
                    bot.name[4] = '\0';
                    clients[1] = bot;
                    break;
                }
                n_game++;
            } else {
                clients[queue_size] = buffer;
                queue_size++;
                if (queue_size == 2) {
                    queue_size = 0;

                    // Check if client are alive before starting the game
                    if (!is_alive(clients[0].pid)) {
                        printf("[Main]The client %d is not alive removing from queue\n",
                               clients[0].pid);
                        clients[0] = clients[1];
                        queue_size = 1;
                        continue;
                    }
                    if (!is_alive(clients[1].pid)) {
                        printf("[Main]The client %d is not alive removing from queue\n",
                               clients[1].pid);
                        queue_size = 1;
                        continue;
                    }

                    if (fork() == 0) {
                        break;
                    }
                    n_game++;
                }
            }
        }

        // Sopra tutto giusto
        if (active == 0) {
            printf("[Main]Stopping server\n");
            pid_t child;
            int status;
            //Tell the queue clients that the server is shutting down
            for (int i = 0; i < 2; i++) {
                kill(clients[i].pid, SIGUSR1);
            }

            //Wait for all the child to terminate their game
            while ((child = waitpid(0, &status, 0)) != -1) {
                printf("[Main]Child %d has been terminated\n", child);
            }
            // Remove every fifo or folder
            clean_everything();
            // Close fifo
            return 0;
        }
    }
    // Child ---------------------------------------------------------

    //Don't need the first_input fifo
    close(fd_fifo_first_input);
    printf("[%d]Starting game\n", n_game);

    // Create a fifo for each client in the folder f4
    for (int i = 0; i < 2; ++i) {
        clients[i].fifo_fd =
                cmd_mkfifo(clients[i].pid, DEFAULT_CLIENTS_DIR, O_WRONLY);
    }
    printf("[%d]Sending Symbols\n", n_game);
    for (int i = 0; i < 2; ++i) {
        struct symbol_info info;
        info.pos = i;
        info.own = symbols[i];
        info.enemy = symbols[!i];
        memcpy(info.enemy_name, clients[!i].name,
               CMD_DEFAULT_STRING_SIZE * sizeof(char));
        cmd_send(clients[i], CMD_SET_SYMBOLS, &info);
    }

    // Creation msg qq
    int msg_qq_input = msgget(ftok(FTOK_MSG, n_game), 0666 | IPC_CREAT);
    if (msg_qq_input == -1) {
        perror("msg queue creation error");
        active = 0;
    }

    // Calculating the size of the shared memory
    printf("[%d]Create shared mem\n", n_game);
    // Create shared memory
    int shm_id = shmget(ftok(FTOK_SMH, n_game), row * column * sizeof(pid_t),
                        IPC_CREAT | S_IRUSR | S_IWUSR | S_IROTH);
    if (shm_id == -1) {
        perror("shared memory creation error");
        active = 0;
    }
    printf("[%d]Attach shared mem (id : %d)\n", n_game, shm_id);
    pid_t *board = (pid_t *) shmat(shm_id, NULL, 0);

    printf("[%d]Create Semaphore\n", n_game);
    int semaphore_id = semaphore_create(ftok(FTOK_SEM, n_game));

    struct game_info gm_info;
    gm_info.row = row;
    gm_info.column = column;
    gm_info.id = n_game;
    printf("[%d]Sending game info (id : %d)\n", n_game, gm_info.id);
    // Broadcast info of shared memory to clients
    cmd_broadcast(clients, CMD_SET_INFO, &gm_info);

    int turn_num = 0; // turno primo client
    struct client_info player;
    struct client_msg client_mv_buffer;
    // TODO add timer
    // Main game loop
    if (active) {
        // Tell the client to read the shared memory
        // legge da memoria condivisa CMD_UPDATE
        // Fill the array with 0
        printf("[%d]Cleaning array\n", n_game);
        clean_array(board, row, column);
        printf("[%d]Sending update board\n", n_game);
        //Tell the client to read the board
        semaphore_set(semaphore_id, 1);
        player = cmd_turn(clients, turn_num); // giocatore che sta facendo la mossa
    }

    int current_time = time(NULL);
    while (active) {
        //Check if the client has a timeout
        if (current_time + time_out <= time(NULL)) {
            cmd_broadcast(clients, CMD_WINNER, &symbols[!turn_num]);
            break;
        }
        // Read incoming msg queue non blocking
        if (msgrcv(msg_qq_input, &client_mv_buffer,
                   sizeof(client_mv_buffer) - sizeof(long), 1,
                   MSG_NOERROR | IPC_NOWAIT) > 0) {
            // Check if a player has abandon the game
            if (client_mv_buffer.move == -1) {
                if (client_mv_buffer.pid == player.pid) {
                    cmd_broadcast(clients, CMD_WINNER, &symbols[!turn_num]);
                } else {
                    cmd_broadcast(clients, CMD_WINNER, &symbols[turn_num]);
                }
                break;
            }
            // Check if sender is the player
            if (client_mv_buffer.pid != player.pid) {
                int error = 0;
                cmd_send(clients[!turn_num], CMD_INPUT_ERROR, &error);
                continue;
            }
            // Block modify to the board if there is a client that is still reading the board
            if (!semaphore_check(semaphore_id, 0)) {
                //TODO check if client is alive by timing
                printf("[%d] Waiting for client to read the board\n", n_game);
                continue;
            }
            // Play the move
            pid_t result = f4_play(board, client_mv_buffer.move, client_mv_buffer.pid,
                                   column, row);
            if (result < -1) {
                // Wrong input
                cmd_send(player, CMD_INPUT_ERROR, &result);
                continue;
            } else if (result == -1) {
                // Matrix piena
                //Tell the clients to read the board
                semaphore_set(semaphore_id, 1);
                char tie_char = '\0';
                cmd_broadcast(clients, CMD_WINNER, &tie_char);
                break;
            } else if (result == player.pid) {

                //Tell the clients to read the board
                semaphore_set(semaphore_id, 1);
                cmd_broadcast(clients, CMD_WINNER, &symbols[turn_num]);
                break;
            }
            //Reset time after a valid move
            current_time = time(NULL);
            turn_num = !turn_num;

            semaphore_set(semaphore_id, 1);
            player = cmd_turn(clients, turn_num);
        }
    }
    semaphore_set(semaphore_id, 1);
    // If the server is not active then tell the clients the server is going
    // offline
    if (!active) {
        kill(clients[0].pid, SIGUSR1);
        kill(clients[1].pid, SIGUSR1);
    }

    //TODO wait for client semaphore for 2 sec

    semaphore_check_time(semaphore_id,0,2);

    sleep(1);

    // Removing shared memory
    shmdt(board);
    shmctl(shm_id, IPC_RMID, NULL);

    // Removing semaphore
    semctl(semaphore_id, 0, IPC_RMID);

    // Removing message queue
    msgctl(msg_qq_input, IPC_RMID, NULL);

    // Close all pipes
    for (int i = 0; i < 2; ++i) {
        printf("[%d] Removing file %s%d\n", n_game, DEFAULT_CLIENTS_DIR,
               clients[i].fifo_fd);
        cmd_rmfifo(clients[i].pid, DEFAULT_CLIENTS_DIR, clients[i].fifo_fd);
    }

    printf("[%d] Game ended\n", n_game);
}

/************************************
 *Matricola VR473680,VR443698
 *Nome e cognome Alex Zanetti,Federico Rossato
 *Data di realizzazione 28 / 4 / 2023
 *************************************/
