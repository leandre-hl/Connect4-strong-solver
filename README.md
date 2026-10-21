[🇫🇷 Français](README.fr.md) | [🇬🇧 English](README.md)

# Connect Four — Strong AI Solver

A Connect Four solver written in C, designed to find optimal moves through game-tree search. The project is a work in progress.

## Features

- **Bitboard representation:** encodes the board using 64-bit integers for efficient move generation and win detection.
- **Negamax search:** explores possible game continuations and evaluates positions from the current player's perspective.
- **Alpha-beta pruning:** reduces the search space by eliminating branches that cannot improve the current decision.
- **Transposition table:** caches previously evaluated positions to avoid redundant computations.
- **Move ordering:** prioritizes central columns to improve search efficiency.
- **Performance statistics:** reports the evaluated score, number of positions explored, execution time and search speed.
- **Terminal interface:** displays the board with colored pieces and allows interactive play.

## Implementation

The solver uses a bitboard representation of the standard 7 × 6 Connect Four board. Each position is represented by two 64-bit integers: one encoding the occupied cells and another encoding the current player's pieces. An additional bit in each column separates adjacent columns, simplifying move generation and alignment detection.

The AI uses a negamax algorithm with alpha-beta pruning. Scores distinguish forced wins, forced losses and draws, while favoring faster wins and delaying inevitable losses.

A transposition table stores position keys and search results, including bounds obtained through alpha-beta pruning. The search also uses null-window searches to narrow down the minimax score.

## Project structure

```text
connect4/
├── main.c
├── Makefile
├── include/
│   ├── ai.h
│   ├── game.h
│   ├── transposition_table.h
│   └── user_interface.h
└── src/
    ├── ai.c
    ├── game.c
    ├── transposition_table.c
    └── user_interface.c
```

- `main.c`: initializes the game and manages the game loop.
- `src/game.c`: implements board representation, move generation, position updates and win detection.
- `src/ai.c`: implements the search algorithm and AI move selection.
- `src/transposition_table.c`: implements the transposition table.
- `src/user_interface.c`: handles board rendering, user input and search statistics.
- `include/`: contains the corresponding header files.
- `Makefile`: defines the build and execution rules.

## Build and run

The project uses GCC and Make.

Compile the executable:

```bash
make compile
```

Run the game:

```bash
make run
```

To compile and run in one command:

```bash
make
```

To remove generated object files and the executable:

```bash
make clean
```

## Current status

The project is still under development. The board representation, game mechanics, terminal interface and core search components have been implemented, but the solver is not yet complete.

Further work includes completing and validating the search algorithm, testing its correctness against known Connect Four positions, and improving its performance on difficult positions.

## Goal

The ultimate goal is to build a strong Connect Four AI capable of solving positions efficiently and finding optimal moves through systematic game-tree search.
