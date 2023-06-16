#include <sys/ipc.h>

void clean_everything();
int semaphore_create(key_t key);
/**
 * Check if the shared memory is safe to write
 * @param sem_id id of the semaphore
 * @return true if every sem is 0 else false
 */
int semaphore_check(int sem_id,int val);

/**
 * Set every semaphore to val
 * @param sem_id id of the semaphore
 * @return the result of semctl
 */
int semaphore_set(int sem_id,int val);



/**
 * Set a semaphore to 0
 * @param sem_id id of the semaphore
 * @param sem_num num of the semaphore to write
 * @return the result of semctl
 */
int semaphore_use(int sem_id,int sem_num);


/************************************
*Matricola VR473680,VR443698
*Nome e cognome Alex Zanetti,Federico Rossato
*Data di realizzazione 30 / 5 / 2023
*************************************/