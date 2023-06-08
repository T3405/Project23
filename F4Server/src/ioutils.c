#include "ioutils.h"
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <dirent.h>

void remove_dir(char* dir){
    DIR *dp;
    struct dirent *ep;
    dp = opendir (dir);
    if (dp != NULL)
    {
        while ((ep = readdir (dp)) != NULL) {
            ep->d_name;
            char tmp_name[strlen(dir)+strlen(ep->d_name)];
            sprintf(tmp_name,"%s%s",dir,ep->d_name);
            remove(tmp_name);
        }

        (void) closedir (dp);
    }

}