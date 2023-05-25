#include "f4logic.h"
#include <stdio.h>
#include <unistd.h>

const int column = 5, row = 6;

int count = 0;

void print_matrix(const int *matrix) {
    printf("\nprint array number %d\n", count++);
    for (int i = c - 1; i >= 0; i--) {
        for (int j = row - 1; j >= 0; j--) {
            printf("%d ", GET_M(matrix, column, j, i));
        }
        printf("\n");
    }
}

int main() {

    pid_t matrix[row * column];

    for (int i = 0; i < row * column; ++i){
        matrix[i] = i;
    }

    print_matrix(matrix);

    clean_array(matrix, row, column);

    print_matrix(matrix);

    for (int i = 0; i < 4; ++i) {
        int play = f4_play(matrix, 2, 1, row, column);
        printf("play return %d\n", play);
    }

    print_matrix(matrix);
}

