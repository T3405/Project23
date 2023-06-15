#include <unistd.h>

/**
 *
 * Use a array as a matrix
 * @param V = matrix
 * @param X = column_size = rows
 * @param Y = n_row
 * @param Z = n_column
 */
#define GET_M(V, X, Y, Z) (V[Z*X+Y])



/**
 * Play the action and check if there is a winner
 * @param matrix matrix to play on
 * @param column action
 * @param player pid_t of the player
 * @param n_column n_row size of the matrix
 * @param n_row n_column size of the matrix
 * @return -1 if there is a invalid plays,0 normal play,pid_t the pid of the winner
 */
int f4_play(pid_t* matrix, int column, pid_t player, int n_column, int n_row);

/**
 * Fill the array all with 0
 * @param matrix the matrix to fill
 */
void clean_array(pid_t* matrix, int n_row, int n_column);


/************************************
*Matricola VR473680,VR443698
*Nome e cognome Alex Zanetti,Federico Rossato
*Data di realizzazione 28 / 4 / 2023
*************************************/