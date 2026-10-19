#include "game.h"

uint64_t top_mask[NCOLS];
uint64_t bottom_mask[NCOLS];
uint64_t bottom_mask_all; /* bitboard with the bottom cells of each column set to 1 */
bool masks_ready = false;

void init_masks(void) {
    if (masks_ready) return;

    bottom_mask_all = 0;

    for (int col = 0; col < NCOLS; col++) {
        top_mask[col] = 1ULL << (NROWS - 1 + col * (NROWS + 1));
        bottom_mask[col] = 1ULL << (col * (NROWS + 1));
        bottom_mask_all |= bottom_mask[col];
    }
    masks_ready = true;
}

game *init_game(void) {
    if (!masks_ready) init_masks();

    game *g = malloc(sizeof(*g));
    assert(g != NULL);

    g->mask = 0;
    g->position = 0;
    g->nb_moves = 0;
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
    g_dest->nb_moves = g_src->nb_moves;
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
    g->nb_moves++;
}

void drop_piece_and_copy(game* g_src, game* g_dest, int col) {
    assert(col >= 0 && col < NCOLS);
    assert(is_valid_move(g_src, col));

    /* switch perspective to the next player */
    g_dest->position = g_src->position ^ g_src->mask;
    g_dest->curr_player = 1 - g_src->curr_player;

    /* add one stone in the chosen column */
    g_dest->mask = g_src->mask | (g_src->mask + bottom_mask[col]);
    g_dest->nb_moves = g_src->nb_moves + 1;
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