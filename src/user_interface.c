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
            printf("   Number of half moves played : %d", g->nb_half_moves);
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
        
        if (scanf("%c", &input) != 1) {
            /* end of input (Ctrl-D): quit instead of looping forever */
            printf("\n");
            exit(EXIT_SUCCESS);
        }

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
