#include "f4logic.h"

/*
 * Funzione modifica matrice e aggiunge il player
 * Controlla se e una giocata valida ritrona -1
 * if winner ritorna il pid del winner
 * else ritorna 0
 *
 * TODO save matrix
 */
int f4_play(pid_t** matrix,int column,pid_t player,int row_size,int column_size){
    if(column >= column_size){
        return -1;
    }

    return 0;
}

void clean_array(pid_t ** matrix,int row_size,int column_size){
    for (int i = 0; i < row_size; ++i)
        for (int j = 0; j < column_size; ++j){
            matrix[i][j] = 0;
        }
}
