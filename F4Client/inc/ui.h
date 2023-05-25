#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/signal.h>
#include <unistd.h>

void clear_screen();

struct winsize get_win_size();

void print_matrix();