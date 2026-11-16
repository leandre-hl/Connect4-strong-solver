#ifndef GAME_H
#define GAME_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

#define NROWS 6
#define NCOLS 7


/* 
Bitboard bit ordering on a 6x7 game : 
row 6 (sentinel)   .  .   .   .   .   .   .
row 5              5  12  19  26  33  40  47
row 4              4  11  18  25  32  39  46
row 3              3  10  17  24  31  38  45
row 2              2   9  16  23  30  37  44
row 1              1   8  15  22  29  36  43
row 0              0   7  14  21  28  35  42
column             0   1   2   3   4   5   6
*/

extern uint64_t top_mask[NCOLS];       /* array of bitboards with the top cell of the column set to 1 */
extern uint64_t bottom_mask[NCOLS];    /* array of bitboards with the bottom cell of the column set to 1 */
extern uint64_t bottom_mask_all;       /* bitboard with the bottom cells of each column set to 1 */
extern uint64_t board_mask;            /* bitborad with all cells set to 1 (but not the sentinel bits) */
extern bool masks_ready;

/* Bitboards use NROWS + 1 bits per column (one sentinel bit on top). */
_Static_assert(NCOLS * (NROWS + 1) <= 64, "board too large for a 64-bit bitboard");

typedef struct {
    uint64_t position;   /* bitboard representing the stones of the current player */
    uint64_t mask;       /* bitboard representing the occupied cells */
    int nb_half_moves;
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
uint64_t compute_winning_positions(game* g);
/* Builds a position from a sequence of columns numbered 1 to 7 (e.g. "4453").
   Returns NULL if the sequence is invalid: unknown column, full column, or moves after a win. */
game* load_position_from_sequence(const char* sequence);

#endif