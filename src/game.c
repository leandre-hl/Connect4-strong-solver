#include "game.h"
#include <stdint.h>

uint64_t top_mask[NCOLS];       /* array of bitboards with the top cell of the column set to 1 */
uint64_t bottom_mask[NCOLS];    /* array of bitboards with the bottom cell of the column set to 1 */
uint64_t bottom_mask_all;       /* bitboard with the bottom cells of each column set to 1 */
uint64_t board_mask;            /* bitborad with all cells set to 1 (but not the sentinel bits) */
bool masks_ready = false;

void init_masks(void) {
    if (masks_ready) return;

    bottom_mask_all = 0;
    board_mask = 0;

    for (int col = 0; col < NCOLS; col++) {
        bottom_mask[col] = 1ULL << (col * (NROWS + 1));
        bottom_mask_all |= bottom_mask[col];
        
        top_mask[col] = bottom_mask[col] << (NROWS - 1);
    }

    board_mask = bottom_mask_all * ((1ULL << NROWS) - 1);

    masks_ready = true;
}

game* init_game(void) {
    if (!masks_ready) init_masks();

    game* g = malloc(sizeof(*g));
    assert(g != NULL);

    g->mask = 0;
    g->position = 0;
    g->nb_half_moves = 0;
    g->curr_player = 0; /* yellow */

    return g;
}

void free_game(game* g) {
    free(g);
}

uint64_t key_from_game(game* g) {
    return g->position + g->mask + bottom_mask_all;
}

void copy_game(game* g_src, game* g_dest) {
    g_dest->mask = g_src->mask;
    g_dest->position = g_src->position;
    g_dest->nb_half_moves = g_src->nb_half_moves;
    g_dest->curr_player = g_src->curr_player;
}

bool is_valid_move(game* g, int col) {
    assert(col >= 0 && col < NCOLS);
    return (g->mask & top_mask[col]) == 0;
}

void drop_piece(game* g, int col) {
    assert(col >= 0 && col < NCOLS);
    assert(is_valid_move(g, col));

    /* switch perspective to the next player */
    g->position ^= g->mask;
    g->curr_player = 1 - g->curr_player;

    /* add one stone in the chosen column */
    g->mask |= g->mask + bottom_mask[col];
    g->nb_half_moves++;
}

void drop_piece_and_copy(game* g_src, game* g_dest, int col) {
    assert(col >= 0 && col < NCOLS);
    assert(is_valid_move(g_src, col));

    /* switch perspective to the next player */
    g_dest->position = g_src->position ^ g_src->mask;
    g_dest->curr_player = 1 - g_src->curr_player;

    /* add one stone in the chosen column */
    g_dest->mask = g_src->mask | (g_src->mask + bottom_mask[col]);
    g_dest->nb_half_moves = g_src->nb_half_moves + 1;
}


bool has_won(game* g) {
    uint64_t m;
    uint64_t pos = g->position ^ g->mask; /* stones of the player who just played */

    /* horizontal */
    m = pos & (pos >> (NROWS + 1));
    if (m & (m >> (2 * (NROWS + 1)))) return true;

    /* vertical */
    m = pos & (pos >> 1);
    if (m & (m >> 2)) return true;

    /* diagonal \ */
    m = pos & (pos >> NROWS);
    if (m & (m >> (2 * NROWS))) return true;

    /* diagonal / */
    m = pos & (pos >> (NROWS + 2));
    if (m & (m >> (2 * (NROWS + 2)))) return true;

    return false;
}

/* return a bitmap of all the winning free spots making an alignment */
uint64_t compute_winning_positions(game* g) {
    // vertical;
    uint64_t r = (g->position << 1) & (g->position << 2) & (g->position << 3);

    //horizontal
    uint64_t p = (g->position << (NROWS + 1)) & (g->position << 2 * (NROWS + 1));
    r |= p & (g->position << 3 * (NROWS + 1));
    r |= p & (g->position >> (NROWS + 1));
    p = (g->position >> (NROWS + 1)) & (g->position >> 2 * (NROWS + 1));
    r |= p & (g->position << (NROWS + 1));
    r |= p & (g->position >> 3 * (NROWS + 1));

    //diagonal 1
    p = (g->position << NROWS) & (g->position << 2 * NROWS);
    r |= p & (g->position << 3 * NROWS);
    r |= p & (g->position >> NROWS);
    p = (g->position >> NROWS) & (g->position >> 2 * NROWS);
    r |= p & (g->position << NROWS);
    r |= p & (g->position >> 3 * NROWS);

    //diagonal 2
    p = (g->position << (NROWS + 2)) & (g->position << 2 * (NROWS + 2));
    r |= p & (g->position << 3 * (NROWS + 2));
    r |= p & (g->position >> (NROWS + 2));
    p = (g->position >> (NROWS + 2)) & (g->position >> 2 * (NROWS + 2));
    r |= p & (g->position << (NROWS + 2));
    r |= p & (g->position >> 3 * (NROWS + 2));

    return r & (board_mask ^ g->mask);
}


game* load_position_from_sequence(const char* sequence) {
    game* g = init_game();

    for (int i = 0; sequence[i] != '\0'; i++) {
        int col = sequence[i] - '1';

        /* invalid: not a column ('1'..'7'), column already full, or a move played after a win */
        if (col < 0 || col >= NCOLS || !is_valid_move(g, col)) {
            free_game(g);
            return NULL;
        }

        drop_piece(g, col);

        /* Possible to win only on the last move */
        if (has_won(g) && sequence[i + 1] != '\0') {
            free_game(g);
            return NULL;
        }
    }

    return g;
}
