#include <sys/ipc.h>
#include <fcntl.h>
#include "errExit.h"
#define MAX_GAMES 254
/**
 * Remove default folders
 */
void clear_folders();

int semaphore_create(key_t key);

/**
 * Check the value of the semaphore
 * @param sem_id id of the semaphore
 * @param val value to check
 * @return true if every sem is val else false
 */
int semaphore_check(int sem_id, int val);

/**
 * Check the if the semaphore has not been semop for delay
 * @param sem_id id of the semaphore
 * @param delay time in seconds
 * @return false if the time has passed else true
 */
int semaphore_check_time(int sem_id, int delay);


/**
 * Set every semaphore to val
 * @param sem_id id of the semaphore
 * @return the result of semctl
 */
int semaphore_set(int sem_id, int val);


/**
 * Set a semaphore to 0
 * @param sem_id id of the semaphore
 * @param sem_num num of the semaphore to write
 * @return the result of semctl
 */
int semaphore_use(int sem_id, int sem_num);


/**
 * Remove every key till the n_games
 */
void remove_key_t_games(int max_games,int size);

/**
 * Remove every IPC with n_game
 */
void remove_key_t_game(int n_game,int size);


/**
 * Get a n_game that is not connected to any ongoing games
 * @param games array of games
 * @param game_pid pid of new created game
 */
int get_safe_game(pid_t* games);

/************************************
*Matricola VR473680,VR443698
*Nome e cognome Alex Zanetti,Federico Rossato
*Data di realizzazione 30 / 5 / 2023
*************************************/