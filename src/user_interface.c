#include "user_interface.h"

char cell_char(game* g, int row, int col) {
    uint64_t bit = UINT64_C(1) << (row + col * (NROWS + 1));

    if (!(g->mask & bit)) {
        return '.';
    }

    uint64_t cur_player_bits;
    if (g->curr_player == 0) {
        cur_player_bits = g->position;
    } else {
        cur_player_bits = g->position ^ g->mask;
    }

    if (cur_player_bits & bit) {
        return 'Y';
    }
    else {
        return 'R';
    }
}

void print_player(int player) {
    if (player == 0) {
        printf(COLOR_YELLOW "YELLOW" COLOR_RESET);
    }
    else {
        printf(COLOR_RED "RED" COLOR_RESET);
    }
}

void print_board(game* g) {
    printf("\n");

    for (int row = NROWS - 1; row >= 0; row--) {
        printf("  ");
        for (int col = 0; col < NCOLS; col++) {
            char c = cell_char(g, row, col);

            printf(COLOR_BLUE "|" COLOR_RESET " ");

            if (c == 'R') {
                printf(COLOR_RED "●" COLOR_RESET);
            } else if (c == 'Y') {
                printf(COLOR_YELLOW "●" COLOR_RESET);
            } else {
                printf("·");
            }

            printf(" ");
        }
        printf(COLOR_BLUE "|  " COLOR_RESET "%d", row);
        
        if (row == 1) {
            printf("   Number of moves played : %d", g->nb_moves);
        }


        if (row == 0) {
            printf("   Player to move : ");
            print_player(g->curr_player);
        }

        printf("\n");
    }

    printf("  ");
    for (int col = 0; col < NCOLS; col++) {
        printf(COLOR_BLUE "+---" COLOR_RESET);
    }
    printf(COLOR_BLUE "+\n" COLOR_RESET);

    printf("   ");
    for (int col = 0; col < NCOLS; col++) {
        printf(" %d  ", col);
    }
    printf("\n\n");
}

int user_strategy(game* g) {
    char input;
    bool valid = false;

    while (!valid) {

        printf("  Enter your move (0-%d) : ", NCOLS - 1);
        
        scanf("%c", &input);

        /* clean buffer */
        int c;
        while ((c = getchar()) != '\n' && c != EOF);

        valid = input >= '0' && input < '0' + NCOLS && is_valid_move(g, input - '0');
        
        if (!valid) {
            printf("  Invalid input\n");
        }
    }

    return (int) (input - '0');
}

void print_ai_specs(int score, int nb_positions_evaluated, double time_taken, int depth) {
    printf("  Score : %d\n", score);
    if (score > 0) {
        printf("  AI has a forced win in %d moves\n", NCOLS*NROWS - score - depth);
    }
    else if (score < 0) {
        printf("  Opponent has a forced win in %d moves\n", NCOLS*NROWS + score - depth);
    }
    else {
        printf("  No forced win for any player\n");
    }
    printf("  Number of positions evaluated : %d\n", nb_positions_evaluated);

    int sec = (int) time_taken;
    int ms = (int) ((time_taken - sec) * 1000);
    printf("  Time taken : %d.%03d seconds\n", sec, ms);

    int kilo_positions_per_sec = (int) (nb_positions_evaluated / time_taken / 1000);
    printf("  Speed : %d kilo-positions/second\n", kilo_positions_per_sec);
}