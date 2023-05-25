#include "f4logic.h"



int check_winner(const pid_t *matrix, pid_t player, int row_size, int column_size) {

    //TODO Fix not working
    // Check horizontally
    for (int row = 0; row < row_size; row++) {
        for (int col = 0; col < column_size - 3; col++) {
            if (GET_M(matrix, column_size, row, col) == player &&
                GET_M(matrix, column_size, row, col + 1) == player &&
                GET_M(matrix, column_size, row, col + 2) == player &&
                GET_M(matrix, column_size, row, col + 3) == player) {
                return player; // Player wins horizontally
            }
        }
    }



    //Working
    // Check vertically
    for (int row = 0; row < row_size - 3; row++) {
        for (int col = 0; col < column_size; col++) {
            if (GET_M(matrix, column_size, row, col) == player &&
                GET_M(matrix, column_size, row + 1, col) == player &&
                GET_M(matrix, column_size, row + 2, col) == player &&
                GET_M(matrix, column_size, row + 3, col) == player) {
                return player; // Player wins vertically
            }
        }
    }


    //TODO Test
    // Check diagonally (top-left to bottom-right)
    for (int row = 0; row < row_size - 3; row++) {
        for (int col = 0; col < column_size - 3; col++) {
            if (GET_M(matrix, column_size, row, col) == player &&
                GET_M(matrix, column_size, row + 1, col+1) == player &&
                GET_M(matrix, column_size, row + 2, col+2) == player &&
                GET_M(matrix, column_size, row + 3, col+3) == player) {
                return player; // Player wins vertically
            }
        }
    }

    //TODO Test
    // Check diagonally (top-right to bottom-left)
    for (int row = 0; row < row_size - 3; row++) {
        for (int col = 3; col < column_size; col++) {
            if (GET_M(matrix, column_size, row, col) == player &&
                GET_M(matrix, column_size, row + 1, col-1) == player &&
                GET_M(matrix, column_size, row + 2, col-2) == player &&
                GET_M(matrix, column_size, row + 3, col-3) == player) {
                return player; // Player wins vertically
            }
        }
    }

    return 0; // Player does not win
}


/*
 * Funzione modifica matrice e aggiunge il player
 * Controlla se e una giocata valida ritorna -1
 * if winner ritorna il pid del winner
 * else ritorna 0
 *
 */
int f4_play(pid_t *matrix, int column, pid_t player, int row_size, int column_size) {

    //Controlla se la colonna esiste
    if (column >= column_size) {
        return -1;
    }

    //Controlla che ci sia spazio sulla colonna

    //TODO fix matrix orientation
    char is_max_col = 0;
    int i;
    for (i = 0; i < column_size; ++i) {

        if (GET_M(matrix,column_size,column,i) == 0) {
            GET_M(matrix,column_size,column,i) = player;
            is_max_col = 1;
            break;
        }
    }
    if (!is_max_col) {
        return -1;
    }
    return check_winner(matrix, player, row_size, column_size);
}


void clean_array(pid_t *matrix, int row_size, int column_size) {
    for (int i = 0; i < row_size; ++i)
        for (int j = 0; j < column_size; ++j) {
            GET_M(matrix, column_size, i, j) = 0;
        }
}


