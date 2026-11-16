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
├── bench/
│   ├── bench.sh
│   └── data/              (test sets, downloaded by `make data`)
├── include/
│   ├── game.h
│   ├── solver.h
│   └── user_interface.h
└── src/
    ├── game.c
    ├── main_play.c
    ├── main_solver.c
    ├── solver.c
    └── user_interface.c
```

- `src/game.c`: board representation, move generation, position updates and win detection.
- `src/user_interface.c`: board rendering and user input.
- `src/main_play.c`: two-player game loop.
- `src/solver.c`: the solver (under construction).
- `src/main_solver.c`: command-line solver following the benchmark protocol (positions on standard input, results on standard output).
- `bench/bench.sh`: benchmark runner (compares the scores, prints a summary table, logs the results in a CSV file).
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

## Benchmark

The benchmark follows the protocol of the tutorial: 6 test sets of 1000 positions. The solver reads one position per line (the played columns, e.g. `4453`) and writes `position score nodes time_in_microseconds`.

```bash
make data                                          # download the test sets into bench/data/
make bench STEP=3 SET=End-Easy LIMIT=100           # quick test: 100 positions, logged as tutorial part 3
make bench STEP=6 LABEL="bitboard"                 # the 6 sets
make bench WEAK=1                                  # weak solver (only the sign of the score is checked)
```

Each run appends one line per set to `bench/results.csv`, which opens in Excel or LibreOffice (`;` separated).
