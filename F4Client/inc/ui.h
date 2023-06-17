#include <unistd.h>
#include "commands.h"

void print_board(const pid_t *board, int sem_id, struct game_info gm_info, struct symbol_info sm_info);

/**
 * Check that string is a number
 * @param string to check
 * @return true or false
 */
int check_string_num(const char *string, ssize_t str_length);

/************************************
 *Matricola VR473680,VR443698
 *Nome e cognome Alex Zanetti,Federico Rossato
 *Data di realizzazione 16 / 6 / 2023
 *************************************/