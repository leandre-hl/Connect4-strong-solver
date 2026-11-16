#ifndef SOLVER_H
#define SOLVER_H

#include <stdbool.h>

#include "game.h"

/* State of a solver. One instance is created per program run and reused for every
   position, so that the transposition table (added later) is allocated only once. */
typedef struct {
    unsigned long long node_count;  /* nodes explored by the last call to solver_solve() */
    game* games[NCOLS*NROWS];   /* game boards copies used to search a position at each depth */
} solver;

solver* solver_new(void);
void solver_free(solver* s);

/* Score of a NON FINAL position (no alignment on the board), from the point of view of the
   player to move, assuming both players play perfectly (blog part 2):
     - positive if the player to move wins: 22 - (number of stones he has played when he wins)
     - 0 for a draw
     - negative if he loses: (number of stones the winner has played) - 22
   Examples: "4455" -> 18, "44455554221" -> -15.

   weak == true: only the sign of the result matters (-1, 0 or +1 are enough).
   A strong solver may ignore this flag: its result is also a valid weak result.

   Must set node_count to 0 on entry, count every call of the recursive function
   (one node per call, including the ones that return immediately), and leave *g unchanged. */
int solver_solve(solver* s, game* g, bool weak);

#endif
