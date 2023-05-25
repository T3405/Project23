#include "ui.h"

void clear_screen(){
    struct winsize size = get_win_size();
    for (int i = 0; i < size.ws_row-1; ++i){
        printf("\n");
    }
}

struct winsize get_win_size(){
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return w;
}