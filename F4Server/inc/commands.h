#include <unistd.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <string.h>

#define DEFAULT_PATH "/tmp/f4server"
#define DEFAULT_DIR "/tmp/f4/"


/**
 * The CMD Code will be used like these
 * Example : 123
 * 123 / 100 = 1 = size of the msg
 * 123 = mtype
 */

#define CMD_MSG_SIZE(X) (sizeof(X)*100)

//TODO think cmd code better
#define CMD_SET_SYMBOL (CMD_MSG_SIZE(char) + 1)
#define CMD_SET_MSG_QQ_ID (CMD_MSG_SIZE(key_t) + 2)
#define CMD_SET_SH_MEM (CMD_MSG_SIZE(struct shared_mem_info) + 3)
#define CMD_UPDATE (4)
#define CMD_TURN (5)

#define CMD_INPUT_ERROR (CMD_MSG_SIZE(int)+6)
#define CMD_WINNER (CMD_MSG_SIZE(char)+7)
#define CMD_SERVER_OFFLINE (8)




struct client_info{
    pid_t pid;
    char mode;
    int fifo_fd;
};


struct client_msg{
    long mtype;
    pid_t pid;
    int move;
};

struct shared_mem_info{
    size_t column;
    size_t row;
    key_t key;
};



/**
 * Send the same message to both clients
 * @param clients clients to be sent client_info[2]
 * @param cmd type of command
 * @param msg message if present
 */
void cmd_broadcast(struct client_info clients[], int cmd, void* msg);

/**
 * Send via message queue a command
 * @param client client to be sent
 * @param cmd type of command
 * @param msg message if present
 */
void cmd_send(struct client_info client, int cmd, void* msg);


/**
 * Send the turn command to the client
 * @param clients Clients
 * @param turn 0 or 1 Turn
 * @return The selected client
 */
struct client_info cmd_turn(struct client_info clients[],int turn);


/*
 * Check if an external process is alive
 * @param pid process
 */
int is_alive(pid_t pid);


int cmd_mkfifo(int pid, char* path,int mode);

int cmd_rmfifo(int pid,char* path,int fd);

int path_size(int id,char* path);
