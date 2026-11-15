#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include "game.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define COLOR_RED     "\033[31m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_RESET   "\033[0m"

void print_player(int player);
void print_board(game* g);

int user_strategy(game* g);

#endif