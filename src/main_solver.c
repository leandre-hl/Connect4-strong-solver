/*
 * Command-line solver following the protocol of Pascal Pons's benchmark
 * (http://blog.gamesolver.org/solving-connect-four/02-test-protocol/):
 *
 *   standard input : one position per line, as the sequence of played columns (1 = left column),
 *                    e.g. "4453"
 *   standard output: one line per position, space separated:
 *                    <position> <score> <number of explored nodes> <time in microseconds>
 *
 * Invalid lines are reported on standard error and skipped.
 * Option -w: weak solver (only the sign of the score is required).
 */

#define _POSIX_C_SOURCE 200809L   /* clock_gettime() is hidden by -std=c11 otherwise */

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "game.h"
#include "solver.h"

/* Current time in whole microseconds (the same unit and truncation as the blog's benchmark). */
static unsigned long long now_us(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (unsigned long long) ts.tv_sec * 1000000ULL + (unsigned long long) ts.tv_nsec / 1000ULL;
}

int main(int argc, char** argv) {
    bool weak = false;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-w") == 0) {
            weak = true;
        } else {
            fprintf(stderr, "usage: %s [-w] < positions > results\n", argv[0]);
            return 2;
        }
    }

    solver* s = solver_new();   /* allocated once for the whole run, not once per position */
    if (s == NULL) {
        fprintf(stderr, "not enough memory\n");
        return 1;
    }

    /* Line buffered: results already computed are not lost if the process is killed (timeout). */
    setvbuf(stdout, NULL, _IOLBF, 0);

    char line[256];
    for (long line_number = 1; fgets(line, sizeof line, stdin) != NULL; line_number++) {
        size_t len = strcspn(line, "\r\n");

        if (line[len] == '\0' && !feof(stdin)) {
            /* no end of line found: the line is longer than the buffer, drop the rest of it */
            int c;
            while ((c = getchar()) != '\n' && c != EOF) {}
            fprintf(stderr, "Line %ld: line too long\n", line_number);
            continue;
        }

        line[len] = '\0';
        if (len == 0) continue;   /* an empty line is NOT the empty board: it would take forever */

        game* g = load_position_from_sequence(line);

        /* the score is only defined for a non final position, so a position that is already won is invalid */
        if (g == NULL || has_won(g)) {
            fprintf(stderr, "Line %ld: invalid position \"%s\"\n", line_number, line);
            if (g != NULL) free_game(g);
            continue;
        }

        unsigned long long start = now_us();
        int score = solver_solve(s, g, weak);
        unsigned long long end = now_us();

        printf("%s %d %llu %llu\n", line, score, s->node_count, end - start);

        free_game(g);
    }

    solver_free(s);
    return 0;
}
