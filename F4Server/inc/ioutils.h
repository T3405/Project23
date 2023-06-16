#include <sys/ipc.h>

void clean_everything();

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


void remove_key_t_games(int n_games,int size);

/************************************
*Matricola VR473680,VR443698
*Nome e cognome Alex Zanetti,Federico Rossato
*Data di realizzazione 30 / 5 / 2023
*************************************/