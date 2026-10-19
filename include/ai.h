#ifndef AI_H
#define AI_H

#include "game.h"
#include "transposition_table.h"
#include "user_interface.h"

#include <stdio.h>
#include <stdlib.h>

void init_games_buffer();
void free_games_buffer();

int ai_strategy(game* g, bool print);

#endif