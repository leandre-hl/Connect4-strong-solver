#include "game.h"
#include "user_interface.h"
#include "ai.h"

#include <assert.h>
#include <stdio.h>
#include <time.h>

int main(void) {
    assert(NROWS <= 10);
    assert(NCOLS * (NROWS + 1) <= 64);
    assert(NROWS + 2 <= 64); /* to avoid overflow */

    srand(time(NULL));

    game* g = init_game();
    init_games_buffer();

    game* g_rand = init_game();

    while (!has_won(g))
    {
        print_board(g);
        if (g->nb_moves >= 23) {
            if (g->curr_player == 1) {
                drop_piece(g, ai_strategy(g, true));
            }
            else {
                drop_piece(g, user_strategy(g));
            }
        }
        else {
            int col = -1;
            bool search = true;
            while (search) {
                col = rand() % NCOLS;
                if (is_valid_move(g, col)) {
                    drop_piece_and_copy(g, g_rand, col);
                    if (!has_won(g_rand)) {
                        search = false;
                    }
                }
            }
            drop_piece(g, col);
        }
    }
    
    print_board(g);
    printf(COLOR_BLUE "  ===== " COLOR_RESET);
    print_player(1 - g->curr_player);
    printf(" WINS !" COLOR_BLUE " =====\n\n" COLOR_RESET);

    free_game(g);
    free_games_buffer();
    return 0;
}