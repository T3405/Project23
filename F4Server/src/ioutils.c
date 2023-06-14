#include "ioutils.h"
#include "commands.h"
#include <dirent.h>

#include <stdio.h>



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