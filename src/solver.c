#include "solver.h"
#include "game.h"

#include <stdlib.h>

solver* solver_new(void) {
    solver* s = malloc(sizeof(*s));
    if (s == NULL) return NULL;

    for (int i = 0; i < NCOLS*NROWS; i++) {
        s->games[i] = malloc(sizeof(*s->games[i]));
        
        if (s->games[i] == NULL) {
            for (int j = 0; j < i; j++) {
                free(s->games[j]);
            }
            free(s);
            return NULL;
        }
    }

    s->node_count = 0;
    return s;
}

void solver_free(solver* s) {
    for (int i = 0; i < NCOLS*NROWS; i++) {
        free(s->games[i]);
    }
    free(s);
}

int negamax(solver* s, game* g, bool weak) {
    s->node_count++;

    /* If game is a draw */
    if (g->nb_half_moves >= NCOLS*NROWS) {  
        return 0;
    }

    int max_moves = (NCOLS * NROWS + 1)/2;
    int max_score = max_moves + 1;

    /* Check for direct winning moves */
    uint64_t winning_positions = compute_winning_positions(g);
    if (winning_positions != 0) {
        for (int col = 0; col < NCOLS; col++) {
            if (!is_valid_move(g, col))
                continue;

            uint64_t move = g->mask + bottom_mask[col];

            if (winning_positions & move) {
                int score;      /* Score must be strictly negative because current player is losing */

                if (weak) {     /* Weak solver */
                    score = 1;
                }
                else {          /* Strong solver */
                    int nb_full_moves = (g->nb_half_moves)/2 + 1;

                    score = max_score - nb_full_moves; 
                }
                
                return score;
            }
        }
    }
    
    /* Else explore every move */
    int score = - max_score;
    int curr_score;
    game* g_copy;

    for(int col = 0; col < NCOLS; col++) {
        if (is_valid_move(g, col)) {
            g_copy = s->games[g->nb_half_moves];
            drop_piece_and_copy(g, g_copy, col);
            
            curr_score = - negamax(s, g_copy, weak);
            if (curr_score > score) {
                score = curr_score;
            }
        }
    }

    return score;
}




int solver_solve(solver* s, game* g, bool weak) {
    s->node_count = 0;

    return negamax(s, g, weak);
}
