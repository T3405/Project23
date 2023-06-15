#include "ioutils.h"
#include "commands.h"
#include <dirent.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <stdio.h>

union semun{
    int val;
    struct semid_ds* buf;
    unsigned short * array;
};

void remove_directory(const char *const path) {
    DIR* directory = opendir(path);
    if(directory == NULL){
        return;
    }
    struct dirent *entry;

    while ((entry= readdir(directory))){
        unlinkat(dirfd(directory),entry->d_name,0);
    }
    rmdir(path);
}

void clean_everything(){
    printf("Unlinking main fifo\n");
    unlink(DEFAULT_PATH);
    printf("Removing default directory\n");
    remove_directory(DEFAULT_CLIENTS_DIR);
}


int semaphore_create(key_t key){
    int sem_id = semget(key, 2, IPC_CREAT | S_IRUSR | S_IWUSR);
    semaphore_set(sem_id,0);
    return sem_id;
}

int semaphore_check(int sem_id){
    unsigned short values[2];
    union semun args;
    args.array = values;
    if(semctl(sem_id,0,GETALL,args)){

    }
    //printf("sem val : {%d,%d}\n",args.array[0],args.array[1]);
    return args.array[0] == 0 && args.array[1] == 0;
}
int semaphore_set(int sem_id,int val){
    unsigned short values[] = {val,val};
    union semun arg;
    arg.array = values;
    //Set sem to 0
    return semctl(sem_id,0,SETALL,arg);
}

int semaphore_use(int sem_id,int sem_num){
    union semun arg;
    arg.val = 0;
    return semctl(sem_id,sem_num,SETVAL,arg);
}


/************************************
*Matricola VR473680,VR443698
*Nome e cognome Alex Zanetti,Federico Rossato
*Data di realizzazione 30 / 5 / 2023
*************************************/