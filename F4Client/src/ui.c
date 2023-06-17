
#include "ui.h"
#include <stdio.h>
#include "f4logic.h"
#include "ioutils.h"

void print_board(const pid_t *board, int sem_id, struct game_info gm_info, struct symbol_info sm_info) {
    if(!semaphore_use(sem_id,sm_info.pos))
        return;
    printf("\n");
    for (int i = 0; i < gm_info.row; ++i) {
        for (int j = 0; j < gm_info.column; ++j) {
            pid_t player_id = GET_M(board, gm_info.row, i, j);
            char print_symbol;
            if (player_id == getpid()) {
                print_symbol = sm_info.own;
            } else if (player_id == 0) {
                print_symbol = ' ';
            } else {
                print_symbol = sm_info.enemy;
            }
            printf("|%c", print_symbol);
        }
        printf("|\n");
    }
}

int check_string_num(const char *string, ssize_t str_length) {
    for (int i = 0; i < str_length-1; ++i){
        if(string[i] < '0' || string[i] > '9'){
            return 0;
        }
    }
    return 1;
}

/************************************
 *Matricola VR473680,VR443698
 *Nome e cognome Alex Zanetti,Federico Rossato
 *Data di realizzazione 16 / 6 / 2023
 *************************************/
