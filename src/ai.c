#include "ai.h"

#include <assert.h>
#include <stdio.h>
#include <time.h>

/* Transposition table */
#define TT_SIZE 1000000 /* 1M */
uint64_t* tt;
bool tt_ready = false;

// Games buffer to store the positions along a searching path
game* ai_games_buffer[NCOLS*NROWS+1];
bool ai_games_buffer_ready = false;

void init_games_buffer() {
    if (ai_games_buffer_ready) free_games_buffer();

    for (int i = 0; i < NCOLS * NROWS + 1; i++) {
        ai_games_buffer[i] = init_game();
    }
    ai_games_buffer_ready = true;
}

void free_games_buffer() {
    if (ai_games_buffer_ready) {
        for (int i = 0; i < NCOLS * NROWS + 1; i++) {
            free_game(ai_games_buffer[i]);
        }
        ai_games_buffer_ready = false;
    }
}

int nb_positions_evaluated = 0; /* number of positions evaluated during the search */


/*
 * Store the best move of a position in best_move if store_best_move = true
   and return the score of the position :
 * - 0 for a draw game
 * - positive score if current player has a forced win. The score is the number
 *   of moves that could be played after winning (the faster the win, the higher the score)
 * - negative score if the oppenent has a forced win. The score is minus the 
 *   number of moves that could be played after losing (the faster the loss, the lower the score)
 * 
 * alpha < beta, score window [alpha, beta] within we are evaluating the position
 */
int negamax(int depth, int alpha, int beta, int* best_move) {
    assert(ai_games_buffer_ready); // ai_games_buffer needs to be initialised
    assert(tt_ready);

    nb_positions_evaluated++; /* Increment number of positions evaluated */

    game* g = ai_games_buffer[depth];

    assert(depth == g->nb_moves); // the game in ai_games_buffer[depth] should correspond to the position after depth moves

    int alpha_orig = alpha; /* original alpha value, used to determine the type of bound to store in the transposition table */
    int beta_orig = beta;   /* original beta  value, used to determine the type of bound to store in the transposition table */

    /* Check for opponent win */
    if (has_won(g)) {
        if (best_move != NULL) {
            *best_move = -1;
        }

        return - (NCOLS*NROWS - (depth - 1));
    }

    /* Check for draw game */
    if (depth == NCOLS*NROWS) {
        if (best_move != NULL) {
            *best_move = -1;
        }

        return 0;
    }

    /* Check transposition table */
    uint64_t key = key_from_game(g);
    uint8_t val = transposition_table_get(key, tt, TT_SIZE); /* Check bound if already computed */
    int bound = (val - 1) % 3;               /* bound stored on the first 2 bits of val */
    int score = (val - 1) / 3 - NCOLS*NROWS; /* score stored with an offset to be able to store negative scores */
    if (val) {
        if (bound == 0) { /* exact score */
            return score;
        }
        else if (bound == 1 && score > alpha) { /* lower bound */
            alpha = score;
        }
        else if (bound == 2 && score < beta) { /* upper bound */
            beta = score;
        }
    }

    /* Fail-soft bounds */
    int max = NCOLS*NROWS - depth; /* upper bound for the current score */

    if (beta > max) {
        beta = max;
    }
    if (alpha < -max) { /* lower bound for the current score */
        alpha = -max;
    }
    if (alpha >= beta) {
        return alpha; /* prune exploration if [alpha, beta] window is empty */
    }


    /* Simulate moves */
    int best_score = -max - 1; /* best score found so far for the current player */
    int best_col = -1;
    
    for (int i = 0; i < NCOLS; i++) {
        /* Move ordering : from the center to the sides */
        int col;
        if (i%2==0) {
            col = (NCOLS-1)/2 - i/2;
        }
        else {
            col = (NCOLS-1)/2 + (i+1)/2;
        }

        if (is_valid_move(g, col)) {
            drop_piece_and_copy(g, ai_games_buffer[depth+1], col);

            /* The score of the current player is the opposite of the score of the opponent */
            int new_score = - negamax(depth + 1, -beta, -alpha, NULL);
            /* No need to have good precision for score better than beta (opponent's score worse than -beta) */
            /* No need to check for score worse than alpha (opponent's score worse better than -alpha)       */
            
            /* Maximise the score for the current player */
            if (new_score > best_score) {
                best_score = new_score;
                best_col = col;
            }

            if (best_score > alpha) {
                alpha = best_score; /* update alpha */
            }

            if (alpha == max) {
                break; /* can not do better than max, so we can stop exploring */
            }

            if (alpha >= beta) {
                break; /* prune exploration if [alpha, beta] window is empty */
            }
        }
    }

    /* Store best move */
    if (best_move != NULL) {
        *best_move = best_col;
    }

    /* Store in transposition table */
    if (best_score <= alpha_orig) {
        bound = 2; /* upper bound */
    }
    else if (best_score >= beta_orig) {
        bound = 1; /* lower bound */
    }
    else {
        bound = 0; /* exact score */
    }

    assert (6*NCOLS*NROWS + 3 <= 255); /* Check that the score with offset and bound can be stored on 8 bits */
    transposition_table_put(key, (best_score + NCOLS*NROWS)*3 + bound + 1, tt, TT_SIZE);

    return best_score;
}

/* Returns the best move in a position */
int ai_strategy(game* g, bool print) {
    clock_t start_time = clock();

    nb_positions_evaluated = 0; /* Reset the number of positions evaluated */

    int depth = g->nb_moves;
    
    copy_game(g, ai_games_buffer[depth]);

    tt = transposition_table_init(TT_SIZE);
    tt_ready = true;

    /* Null window search */
    int g = 0;
    int upper = NCOLS*NROWS - depth;
    int lower = -upper;
    while (lower < upper) {
        int alpha;
        if (g == lower) {
            alpha = g;
        }
        else {
            alpha = g - 1;
        }
        g = negamax(depth, alpha, alpha + 1, NULL); /* Use a null window to know if the score is grater or smaller than med */
        if (g <= alpha) {
            upper = g;
        }
        else {
            lower = g;
        }
    }

    int best_move;

    int score = negamax(depth, -T, T, &best_move);

    transposition_table_free(tt);
    tt_ready = false;

    clock_t end_time = clock();
    double elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    if (print) {
        print_ai_specs(score, nb_positions_evaluated, elapsed_time, depth);
    }

    return best_move;
}
