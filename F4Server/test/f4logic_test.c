#include "f4logic.h"
#include <stdio.h>
#include <unistd.h>

const int n_column = 5, n_row = 6;

int count = 0;

void print_matrix(const int *matrix) {
    printf("\nprint array number %d\n", count++);
    for (int i = 0; i < n_row; ++i){
        for (int j = 0; j < n_column; ++j){
            printf("%d ", GET_M(matrix, n_row, i, j));
        }
        printf("\n");
    }
}

void print_array(const int *array) {
    for (int i = 0; i < n_column * n_row; ++i) {
        printf("%d,", array[i]);
    }
    printf("\n");
}

int main() {

    pid_t matrix[n_row * n_column];

    for (int i = 0; i < n_row * n_column; ++i) {
        matrix[i] = i;
    }
    print_array(matrix);

    print_matrix(matrix);

    clean_array(matrix, n_row, n_column);

    print_matrix(matrix);

    int play;
    for (int i = 0; i < 4; ++i){
        play = f4_play(matrix, 4, 9, n_column, n_row);
    }
    printf("play vertical %d\n", play);
    print_matrix(matrix);

    clean_array(matrix, n_row, n_column);
    print_matrix(matrix);
    for (int j = 0; j < n_column; ++j) {
        for (int i = 0; i < 2; ++i) {
            play = f4_play(matrix, j, 9, n_column, n_row);
        }
        printf("play horizontal %d\n", play);
    }


    print_matrix(matrix);
    print_array(matrix);


}

/************************************
*Matricola VR473680,VR443698
*Nome e cognome Alex Zanetti,Federico Rossato
*Data di realizzazione 30 / 5 / 2023
*************************************/