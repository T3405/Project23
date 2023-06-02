#include "f4logic.h"


int check_winner(const pid_t *matrix, int n_column, int n_row) {
    pid_t cell;

    //Vertical
    for (int j = 0; j < n_column; ++j){
        for (int i = 0; i < n_row - 3; ++i){
            cell = GET_M(matrix, n_row, i, j);
            if(cell == 0){
                continue;
            }

            if(cell == GET_M(matrix, n_row, i + 1, j) &&
               cell == GET_M(matrix, n_row, i + 2, j) &&
               cell == GET_M(matrix, n_row, i + 3, j)){
                return cell;
            }

        }
    }

    //Horizontal
    for (int i = 0; i < n_row; ++i){
        for (int j = 0; j < n_column - 3; ++j){
            cell = GET_M(matrix, n_row, i, j);

            if(cell == 0){
                continue;
            }

            if(cell == GET_M(matrix, n_row, i, j + 1) &&
               cell == GET_M(matrix, n_row, i, j + 2) &&
               cell == GET_M(matrix, n_row, i, j + 3)){
                return cell;
            }

        }
    }

    //Diagonale destra
    for (int i = 0; i < n_row - 3; i++) {
        for (int j = 0; j < n_column - 3; j++) {
            cell = GET_M(matrix, n_row, i, j);
            if (cell != 0 && cell == GET_M(matrix, n_row, i+1, j+1) &&
                cell == GET_M(matrix, n_row, i+2, j+2) &&
                cell == GET_M(matrix, n_row, i+3, j+3)) {
                return cell;
            }
        }
    }

    //Diagonale sinistra
    for (int i = 3; i < n_row; i++) {
        for (int j = 0; j < n_column - 3; j++) {
            cell = GET_M(matrix, n_row, i, j);
            if (cell != 0 && cell == GET_M(matrix, n_row, i-1, j+1) &&
                cell == GET_M(matrix, n_row, i-2, j+2) &&
                cell == GET_M(matrix, n_row, i-3, j+3)) {
                return cell;
            }
        }
    }
    return 0;
}


/*
 * Funzione modifica matrice e aggiunge il player
 * Controlla se e una giocata valida ritorna -1
 * if winner ritorna il pid del winner
 * else ritorna ZERO
 *
 */

//size_x = lenght of x
//n_x = size of x
int f4_play(pid_t *matrix, int column, pid_t player, int n_column, int n_row) {

    //Controlla se la colonna esiste
    if (column >= n_column) {
        return -1;
    }

    //Controlla che ci sia spazio sulla colonna

    //TODO fix matrix orientation

    char is_max_col = 0;
    int i;
    for (i = n_row - 1; i >= 0; i--) {
        if (GET_M(matrix, n_row, i, column) == 0) {
            GET_M(matrix, n_row, i, column) = player;
            is_max_col = 1;
            break;
        }
    }
    if (!is_max_col) {
        return -1;
    }
    return check_winner(matrix, n_column, n_row);
}


void clean_array(pid_t *matrix, int n_row, int n_column) {
    for (int i = 0; i < n_row * n_column; ++i){
        matrix[i] = 0;
    }
}


