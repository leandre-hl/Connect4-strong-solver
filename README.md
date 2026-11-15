[🇫🇷 Français](README.fr.md) | [🇬🇧 English](README.md)

# Connect Four — Strong AI Solver

A Connect Four solver written in C. The project is being rewritten from scratch, following Pascal Pons's tutorial [Solving Connect 4: how to build a perfect AI](http://blog.gamesolver.org/solving-connect-four/).

## Current status

- **Done:** bitboard board representation (7 × 6, 64-bit integers) and a two-player game in the terminal.
- **Next:** validate the bitboards against a naive implementation, then rebuild the solver step by step.

## Roadmap

1. Bitboard `Position` with a test suite checked against a naive 2D-array implementation.
2. Solver, one tutorial part at a time: negamax, alpha-beta, move ordering, transposition table, iterative deepening and null window, skipping losing moves, move sorting, optimized transposition table, lower bounds.
3. Benchmark based on the tutorial's protocol: 6 test sets of 1000 positions, one position per line on standard input, `position score nodes time` on standard output.
4. Command-line player using the solver.

## Project structure

```text
connect4/
├── Makefile
├── include/
│   ├── game.h
│   └── user_interface.h
└── src/
    ├── game.c
    ├── main_play.c
    └── user_interface.c
```

- `src/game.c`: board representation, move generation, position updates and win detection.
- `src/user_interface.c`: board rendering and user input.
- `src/main_play.c`: two-player game loop.
- `include/`: header files.
- `Makefile`: build rules.

## Build and run

The project uses GCC and Make.

```bash
make            # build the game (optimized) into build/release/connect4
make run        # build and play
make debug      # build with -O0 and the address/undefined-behavior sanitizers
make clean      # remove the build/ directory
```
