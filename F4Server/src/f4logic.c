#include "f4logic.h"
//const int n_column = 5, n_row = 6;
int check_winner(const pid_t *matrix, pid_t player, int n_column, int n_row) {
    pid_t cell;

    //Vertical good

    for (int j = 0; j < n_column; ++j){
        for (int i = 0; i < n_row - 3; ++i){
            cell = GET_M(matrix, n_row, i, j);
          //  printf("c %d , r %d \ncell : %d\n",j,i,cell);
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
    // 1 3

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
/*
    // Check for horizontal wins
    for (i = 0; i < n_column; i++) {
        for (j = 0; j < n_row - 3; j++) {
            cell = GET_M(matrix, n_row, i, j);
            if (cell != 0 && cell == GET_M(matrix, n_row, i, j+1) &&
            cell == GET_M(matrix, n_row, i, j+2) &&
            cell == GET_M(matrix, n_row, i, j+3)) {
                return cell;
            }
        }
    }



    // Check for diagonal wins (top-left to bottom-right)
    for (i = 0; i < n_column - 3; i++) {
        for (j = 0; j < n_row - 3; j++) {
            cell = GET_M(matrix, n_row, i, j);
            if (cell != 0 && cell == GET_M(matrix, n_row, i+1, j+1) &&
                cell == GET_M(matrix, n_row, i+2, j+2) &&
                cell == GET_M(matrix, n_row, i+3, j+3)) {
                return cell;
            }
        }
    }

    // Check for diagonal wins (bottom-left to top-right)
    for (i = 3; i < n_column; i++) {
        for (j = 0; j < n_row - 3; j++) {
            cell = GET_M(matrix, n_row, i, j);
            if (cell != 0 && cell == GET_M(matrix, n_row, i-1, j+1) &&
                cell == GET_M(matrix, n_row, i-2, j+2) &&
                cell == GET_M(matrix, n_row, i-3, j+3)) {
                return cell;
            }
        }
    }

    // No winner found
    */
    return 0;
}


/*
 * Funzione modifica matrice e aggiunge il player
 * Controlla se e una giocata valida ritorna -1
 * if winner ritorna il pid del winner
 * else ritorna 0
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
    return check_winner(matrix, player, n_column, n_row);
}


void clean_array(pid_t *matrix, int n_row, int n_column) {
    for (int i = 0; i < n_row * n_column; ++i){
        matrix[i] = 0;
    }
}


