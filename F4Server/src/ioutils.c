#include "ioutils.h"
#include "commands.h"

#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <errno.h>
#include <sys/wait.h>

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

void remove_directory(const char *const path) {
    DIR *directory = opendir(path);
    if (directory == NULL) {
        return;
    }
    struct dirent *entry;

    while ((entry = readdir(directory))) {
        unlinkat(dirfd(directory), entry->d_name, 0);
    }
    rmdir(path);
}

void remove_key_t_games(int max_games,int size){
    for (int i = 0; i < max_games; ++i) {
        remove_key_t_game(i,size);
    }
}

void remove_key_t_game(int n_game,int size){
    int sem_id = semget(ftok(FTOK_SEM,n_game),2,0666);
    semctl(sem_id,0,IPC_RMID);
    int msg_qq = msgget(ftok(FTOK_MSG,n_game),0666);
    msgctl(msg_qq,IPC_RMID,NULL);
    int shm_mem = shmget(ftok(FTOK_SMH,n_game),size,0666);
    shmctl(shm_mem,IPC_RMID,NULL);
}
void clear_folders() {
    printf("Unlinking main fifo\n");
    unlink(DEFAULT_PATH);
    printf("Removing default directory\n");
    remove_directory(DEFAULT_CLIENTS_DIR);
}

int semaphore_create(key_t key) {
    int sem_id = semget(key, 2, IPC_CREAT | S_IRUSR | S_IWUSR);
    semaphore_set(sem_id, 0);
    return sem_id;
}

int semaphore_check(int sem_id, int val) {
    unsigned short values[2];
    union semun args;
    args.array = values;
    if (semctl(sem_id, 0, GETALL, args)) {
    }
#ifdef DEBUG
    printf("sem val : {%d,%d}\n", args.array[0], args.array[1]);
#endif
    return args.array[0] == val && args.array[1] == val;
}

int semaphore_check_time(int sem_id, int delay) {
    struct semid_ds ds;
    union semun arg;
    arg.buf = &ds;
    semctl(sem_id,0,IPC_STAT,arg);
    return ds.sem_otime + delay <= time(NULL);
}

int semaphore_set(int sem_id, int val) {
    unsigned short values[] = {val, val};
    union semun arg;
    arg.array = values;
    // Set sem to 0
    return semctl(sem_id, 0, SETALL, arg);
}

int semaphore_use(int sem_id, int sem_num) {
    struct sembuf sops;
    sops.sem_num = sem_num;
    sops.sem_op = -1;
    sops.sem_flg = O_NONBLOCK;
    //Check if sem is 1
    if (semop(sem_id, &sops, 1) == -1) {
        if (errno == EAGAIN) {
            return 0;
        }
        errExit("semaphore error");
    }
    return 1;
}


int get_safe_game(pid_t* games){
    for (size_t i = 0; i < MAX_GAMES; i++)
    {
        if(games[i] == 0){
            return i+1;
        }
        int status;
        pid_t result = waitpid(games[i], &status, WNOHANG);
        if(result != 0){
            return i+1;
        }
    }
    return -1;
}


/************************************
 *Matricola VR473680,VR443698
 *Nome e cognome Alex Zanetti,Federico Rossato
 *Data di realizzazione 30 / 5 / 2023
 *************************************/
