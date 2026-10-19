# Puissance 4 — Solveur fort IA

Un solveur de Puissance 4 écrit en C, conçu pour déterminer les meilleurs coups grâce à l'exploration de l'arbre de jeu. Le projet est encore en cours de développement.

## Fonctionnalités

- **Représentation par bitboards :** encode le plateau à l'aide d'entiers de 64 bits pour accélérer la génération des coups et la détection des alignements.
- **Recherche Negamax :** explore les continuations possibles et évalue les positions du point de vue du joueur courant.
- **Élagage alpha-bêta :** réduit l'espace de recherche en éliminant les branches qui ne peuvent pas améliorer la décision actuelle.
- **Table de transpositions :** mémorise les positions déjà évaluées afin d'éviter les calculs redondants.
- **Ordonnancement des coups :** privilégie les colonnes centrales pour améliorer l'efficacité de la recherche.
- **Statistiques de performance :** affiche le score évalué, le nombre de positions explorées, le temps d'exécution et la vitesse de recherche.
- **Interface en terminal :** affiche le plateau avec des pions colorés et permet de jouer une partie de manière interactive.

## Implémentation

Le solveur utilise une représentation par bitboards du plateau standard de Puissance 4, composé de 7 colonnes et de 6 lignes. Chaque position est représentée par deux entiers de 64 bits : l'un encode les cases occupées, l'autre les pions du joueur courant. Un bit supplémentaire dans chaque colonne sépare les colonnes adjacentes, ce qui simplifie la génération des coups et la détection des alignements.

L'IA utilise l'algorithme Negamax avec élagage alpha-bêta. Les scores permettent de distinguer les victoires forcées, les défaites forcées et les matchs nuls, tout en favorisant les victoires rapides et en retardant les défaites inévitables.

Une table de transpositions stocke les clés des positions et les résultats de recherche, notamment les bornes obtenues grâce à l'élagage alpha-bêta. La recherche utilise également des recherches à fenêtre nulle pour affiner l'estimation du score minimax.

## Structure du projet

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

- `main.c` : initialise la partie et gère la boucle de jeu.
- `src/game.c` : implémente la représentation du plateau, la génération des coups, la mise à jour des positions et la détection des victoires.
- `src/ai.c` : implémente l'algorithme de recherche et la sélection des coups par l'IA.
- `src/transposition_table.c` : implémente la table de transpositions.
- `src/user_interface.c` : gère l'affichage du plateau, la saisie utilisateur et les statistiques de recherche.
- `include/` : contient les fichiers d'en-tête correspondants.
- `Makefile` : définit les règles de compilation et d'exécution.

## Compilation et exécution

Le projet utilise GCC et Make.

Compiler l'exécutable :

```bash
make compile
```

Lancer le jeu :

```bash
make run
```

Pour compiler et lancer le programme en une seule commande :

```bash
make
```

Pour supprimer les fichiers objets générés et l'exécutable :

```bash
make clean
```

## État actuel du projet

Le projet est encore en développement. La représentation du plateau, les mécanismes du jeu, l'interface en terminal et les principaux composants de recherche ont été implémentés, mais le solveur n'est pas encore terminé.

Les prochaines étapes consistent à finaliser et valider l'algorithme de recherche, à tester sa correction sur des positions de Puissance 4 connues et à améliorer ses performances sur les positions difficiles.

## Objectif

L'objectif final est de développer une IA de Puissance 4 performante, capable de résoudre efficacement des positions et de trouver des coups optimaux grâce à une exploration systématique de l'arbre de jeu.