#include "game.h"
#include "user_interface.h"

#include <stdio.h>

int main(void) {
    game* g = init_game();

    /* Two human players, until a win or a full board. */
    while (!has_won(g) && g->nb_half_moves < NROWS * NCOLS) {
        print_board(g);
        drop_piece(g, user_strategy(g));
    }

    print_board(g);
    if (has_won(g)) {
        printf(COLOR_BLUE "  ===== " COLOR_RESET);
        print_player(1 - g->curr_player);
        printf(" WINS !" COLOR_BLUE " =====\n\n" COLOR_RESET);
    } else {
        printf(COLOR_BLUE "  ===== DRAW =====\n\n" COLOR_RESET);
    }

    free_game(g);
    return 0;
}
