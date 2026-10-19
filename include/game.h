#ifndef GAME_H
#define GAME_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

#define NROWS 6 /* Must have NROWS <= 10 */
#define NCOLS 7 /* Must have NROWS * (NCOLS + 1) <= 64 for bitboards representation */

typedef struct {
    uint64_t position;   /* bitboard representing the stones of the current player */
    uint64_t mask;       /* bitboard representing the occupied cells */
    int nb_moves;
    int curr_player;     /* 0 for yellow (first player), 1 for red */
} game;

game* init_game(void);
void free_game(game* g);
void copy_game(game* g_src, game* g_dest);

uint64_t key_from_game(game* g);

bool is_valid_move(game* g, int col);
void drop_piece(game* g, int col);
void drop_piece_and_copy(game* g_src, game* g_dest, int col);
bool has_won(game* g);

#endif