#include <unistd.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <string.h>

#define DEFAULT_PATH "/tmp/f4server"
#define DEFAULT_CLIENTS_DIR "/tmp/f4/"
#define FTOK_MSG DEFAULT_PATH
#define FTOK_SMH DEFAULT_CLIENTS_DIR
#define FTOK_SEM DEFAULT_CLIENTS_DIR


/**
 * The CMD Code will be used like these
 * Example : 123
 * 123 / 100 = 1 = size of the msg
 * 123 = type
 */

#define CMD_MSG_SIZE(X) (sizeof(X)*100)
#define CMD_DEFAULT_STRING_SIZE 50
#define CMD_SET_SYMBOLS (CMD_MSG_SIZE(struct symbol_info) + 1)
#define CMD_SET_INFO (CMD_MSG_SIZE(struct game_info) + 2)
#define CMD_UPDATE (3)
#define CMD_TURN (4)

#define CMD_INPUT_ERROR (CMD_MSG_SIZE(int)+5)
#define CMD_WINNER (CMD_MSG_SIZE(char)+6)



struct client_info {
    pid_t pid;
    char mode;
    int fifo_fd;
    char name[CMD_DEFAULT_STRING_SIZE];
};

struct symbol_info {
    int pos;
    char own;
    char enemy;
    char enemy_name[CMD_DEFAULT_STRING_SIZE];
};

struct client_msg {
    long mtype;
    pid_t pid;
    int move;
};

struct game_info {
    size_t column;
    size_t row;
    int id;

};


/**
 * Send the same message to both clients
 * @param clients clients to be sent client_info[2]
 * @param cmd type of command
 * @param msg message if present
 */
void cmd_broadcast(struct client_info clients[], int cmd, void *msg);

/**
 * Send via message queue a command
 * @param client client to be sent
 * @param cmd type of command
 * @param msg message if present
 */
void cmd_send(struct client_info client, int cmd, void *msg);


/**
 * Send the turn command to the client
 * @param clients Clients
 * @param turn 0 or 1 Turn
 * @return The selected client
 */
struct client_info cmd_turn(struct client_info clients[], int turn);


/**
 * Check if an external process is alive
 * @param pid process
 * @return false if the process does not exists
 */
int is_alive(pid_t pid);


int cmd_mkfifo(int pid, char *path, int mode);

int cmd_rmfifo(int pid, char *path, int fd);

int path_size(int id, char *path);

/************************************
*Matricola VR473680
*Nome e cognome Alex Zanetti
*Data di realizzazione 28 / 4 / 2023
*************************************/