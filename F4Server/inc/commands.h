#include <unistd.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <string.h>

#define DEFAULT_PATH "/tmp/f4server"


/**
 * The CMD Code will be used like these
 * Example : 123
 * 123 / 100 = 1 = size of the msg
 * 123 = mtype
 */


union semun {
    int val;
    struct semid_ds *buf;
    unsigned short  *array;
};

#define CMD_MSG_SIZE(X) (sizeof(X)*100)

//TODO think cmd code better
#define CMD_SET_SYMBOL (CMD_MSG_SIZE(char) + 0)
#define CMD_SET_MSG_QQ_ID (CMD_MSG_SIZE(key_t) + CMD_MSG_SIZE(int)*2 + 1)
#define CMD_SET_SH_MEM (CMD_MSG_SIZE(struct shared_mem_info) + 2)
#define CMD_UPDATE (3)
#define CMD_TURN (4)

#define CMD_INPUT_ERROR (6)
#define CMD_WINNER (CMD_MSG_SIZE(char)+7)


#define CMD_CLI_ACTION (CMD_MSG_SIZE(struct client_action) + 5)


struct client_info{
    pid_t pid;
    key_t message_qq;
    char mode;
};


struct msg_buffer{
    long mtype;
    void *msg;
};

struct shared_mem_info{
    size_t column;
    size_t row;
    key_t key;
};

/**
 * Client action
 * pid = Process id of the sending process
 * n_column = if >= 0 then number of the play n_column
 * else -1 if abandon
 */
struct client_action{
    pid_t pid;
    int column;
};


/**
 * Send the same message to both clients
 * @param clients clients to be sent client_info[2]
 * @param cmd type of command
 * @param msg message if present
 */
void cmd_broadcast(struct client_info clients[], long cmd, void* msg);

/**
 * Send via message queue a command
 * @param client client to be sent
 * @param cmd type of command
 * @param msg message if present
 */
void cmd_send(struct client_info client, long cmd, void* msg);


/**
 * Send the turn command to the client
 * @param clients Clients
 * @param turn 0 or 1 Turn
 * @return The selected client
 */
struct client_info cmd_turn(struct client_info clients[],int turn);

/**
 * GET THE SIZE OF THE STRUCT msg_buffer - long
 */
ssize_t get_msg_size(long code);

/*
 * Check if an external process is alive
 * @param pid process
 */
int is_alive(pid_t pid);