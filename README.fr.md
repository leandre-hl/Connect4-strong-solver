[🇫🇷 Français](README.fr.md) | [🇬🇧 English](README.md)

# Puissance 4 — IA Solveur fort

Un solveur de Puissance 4 écrit en C. Le projet est en cours de réécriture complète, en suivant le tutoriel de Pascal Pons [Solving Connect 4: how to build a perfect AI](http://blog.gamesolver.org/solving-connect-four/).

## État actuel

- **Fait :** représentation du plateau par bitboards (7 × 6, entiers de 64 bits) et partie à deux joueurs dans le terminal.
- **Prochaine étape :** valider les bitboards par rapport à une implémentation naïve, puis reconstruire le solveur pas à pas.

## Feuille de route

1. `Position` en bitboards, avec une suite de tests comparée à une implémentation naïve à tableau 2D.
2. Solveur, une partie du tutoriel à la fois : negamax, alpha-bêta, ordre des coups, table de transposition, approfondissement itératif et fenêtre nulle, élimination des coups perdants, tri des coups, table de transposition optimisée, bornes basses.
3. Benchmark basé sur le protocole du tutoriel : 6 jeux de 1000 positions, une position par ligne sur l'entrée standard, `position score nœuds temps` sur la sortie standard.
4. Joueur en ligne de commande utilisant le solveur.

## Structure du projet

```text
connect4/
├── Makefile
├── bench/
│   ├── bench.sh
│   └── data/              (jeux de test, téléchargés par `make data`)
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

- `src/game.c` : représentation du plateau, génération des coups, mise à jour des positions et détection des victoires.
- `src/user_interface.c` : affichage du plateau et saisie utilisateur.
- `src/main_play.c` : boucle de jeu à deux joueurs.
- `src/solver.c` : le solveur (en construction).
- `src/main_solver.c` : solveur en ligne de commande suivant le protocole du benchmark (positions sur l'entrée standard, résultats sur la sortie standard).
- `bench/bench.sh` : lanceur du benchmark (compare les scores, affiche un tableau récapitulatif, enregistre les résultats dans un fichier CSV).
- `include/` : fichiers d'en-tête.
- `Makefile` : règles de compilation.

## Compilation et exécution

Le projet utilise GCC et Make.

```bash
make            # compile le jeu (optimisé) dans build/release/connect4
make run        # compile et lance une partie
make debug      # compile en -O0 avec les sanitizers address/undefined
make clean      # supprime le dossier build/
```

## Benchmark

Le benchmark suit le protocole du tutoriel : 6 jeux de test de 1000 positions. Le solveur lit une position par ligne (les colonnes jouées, par exemple `4453`) et écrit `position score nœuds temps_en_microsecondes`.

```bash
make data                                          # télécharge les jeux de test dans bench/data/
make bench STEP=3 SET=End-Easy LIMIT=100           # test rapide : 100 positions, enregistré comme partie 3 du tutoriel
make bench STEP=6 LABEL="bitboard"                 # les 6 jeux
make bench WEAK=1                                  # solveur faible (seul le signe du score est vérifié)
```

Chaque lancement ajoute une ligne par jeu dans `bench/results.csv`, qui s'ouvre dans Excel ou LibreOffice (séparateur `;`).
