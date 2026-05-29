# Projet INFO4108 — Jeu de d'Alembert

> **Documentation technique complète**

| | |
|---|---|
| **Étudiante** | Paola Stéphanie Ngameni Djomdjou |
| **Numéro étudiant** | A0229070 |
| **Session** | Hiver 2026 |
| **Matière** | INFO4108 — Programmation Unix |
| **Enseignante / Enseignant** | Professeur Éric Hervet |

---

## Compilation et exécution rapide

```bash
# Compiler le projet
make

# Lancer le programme
./dalembert

# Nettoyer les fichiers compilés 
make clean

```

---

## Sommaire

1. [Introduction](#1-introduction)
2. [Règles du jeu de d'Alembert](#2-règles-du-jeu-de-dalembert)
3. [Méthode de développement](#3-méthode-de-développement)
4. [Architecture du programme](#4-architecture-du-programme)
5. [Description détaillée des fichiers](#5-description-détaillée-des-fichiers)
6. [Communication inter-processus](#6-communication-inter-processus)
7. [Interface graphique — Raylib](#7-interface-graphique--raylib)
8. [Fonctionnalité de vitesse dynamique](#8-fonctionnalité-de-vitesse-dynamique)
9. [Flux d'exécution](#9-flux-dexécution)
10. [Prérequis et installation](#10-prérequis-et-installation)
11. [Tests réalisés](#11-tests-réalisés)
12. [Difficultés rencontrées et solutions](#12-difficultés-rencontrées-et-solutions)
13. [Améliorations possibles](#13-améliorations-possibles)
14. [Sources et outils utilisés](#14-sources-et-outils-utilisés)
15. [Conclusion](#15-conclusion)

---

## 1. Introduction

Ce projet s'inscrit dans le cadre du cours **INFO4108 — Programmation Unix**, dont l'objectif est de mettre en pratique les concepts fondamentaux de la programmation système sous Linux :

- création et gestion de processus via `fork()`
- communication inter-processus via `pipe()`, `read()` et `write()`
- synchronisation par blocage naturel (`read()` bloquant)
- persistance des données dans un fichier (`score.txt`)
- compilation modulaire avec `Makefile`

Le sujet imposait l'implémentation du **jeu de d'Alembert**, jeu basé sur des déplacements aléatoires sur une grille 11×11, opposant deux processus fils jouant automatiquement l'un contre l'autre.

Ce projet a été réalisé **individuellement**. J'ai choisi d'implémenter une interface graphique moderne en C++ avec la bibliothèque **Raylib**, tout en respectant strictement l'ensemble des contraintes système imposées par le sujet. Une fonctionnalité supplémentaire de **contrôle de vitesse dynamique** a également été ajoutée pour améliorer l'expérience utilisateur.

---

## 2. Règles du jeu de d'Alembert (Rappel)

### 2.1 Matériel de jeu

- Une grille carrée de **11 × 11 cases** (121 cases au total).
- **Deux pions**, démarrant tous deux depuis la case centrale **(6, 6)**.
- Deux séries de jetons numérotés **{-2, -1, 0, 1, 2}** — une série pour l'axe `dx` (abscisse), une pour l'axe `dy` (ordonnée).
- Des **cases piégées** marquées d'un X, dont les positions sont définies dans le code source (`is_trap_cell()`).

### 2.2 Déroulement d'un tour

À chaque tour, le joueur actif :

1. Tire aléatoirement un jeton `dx` et un jeton `dy` dans `{-2, -1, 0, 1, 2}`.
2. Calcule la case cible : `(x + dx, y + dy)`.
3. Applique les règles suivantes **dans l'ordre strict** :

| Priorité | Condition | Action |
|:---:|---|---|
| 1 | Case hors grille (`x < 1`, `x > 11`, `y < 1` ou `y > 11`) | Le pion **ne bouge pas**. |
| 2 | Case piégée (marquée X) | Le pion est ramené en **(1, 1)** — pas de victoire. |
| 3 | Déplacement valide | Le pion se déplace sur la case cible. |
| 4 | Collision avec l'adversaire | L'adversaire est renvoyé en **(1, 1)**. |
| 5 | Le joueur atteint (1,1) ou (11,11) normalement | **Victoire** du joueur actif. |

4. Les jetons sont remis dans leurs séries respectives.

### 2.3 Condition de victoire

Un joueur gagne lorsqu'il atteint la case **(1,1)** ou **(11,11)** à la suite d'un tirage normal.

> ⚠️ **Important :** être renvoyé en (1,1) par une case X ou par collision avec l'adversaire **ne constitue pas une victoire**.

---

## 3. Méthode de développement

Le développement s'est fait de manière progressive et structurée, par étapes validées l'une après l'autre.

### Étape 1 — Logique du jeu
Implémentation de la grille 11×11, des déplacements, des cases piégées et des conditions de victoire dans `game.cpp` / `game.h`.

### Étape 2 — Partie système UNIX
Ajout de `fork()` et des `pipe()` pour la création des processus et la communication inter-processus dans `main.cpp`.

### Étape 3 — Tests en terminal
Validation complète de la logique via `std::cout` avant d'intégrer toute interface graphique.

### Étape 4 — Exploration ncurses
Expérimentation d'une interface textuelle améliorée avec `ncurses` (fichier `test_ncurse.cpp` conservé à titre de référence dans le dépôt).

### Étape 5 — Interface graphique Raylib
Remplacement de l'interface textuelle par une interface graphique complète avec **Raylib**, choisie pour sa légèreté, sa compatibilité Linux native et sa simplicité d'intégration en C++.

### Étape 6 — Vitesse dynamique
Ajout d'un système de contrôle de vitesse permettant de modifier la cadence du jeu au lancement et en cours de partie.

---

## 4. Architecture du programme

### 4.1 Arborescence des fichiers

```
JeuDed-Alembert-main/
├── include/
│   ├── game.h           # Structures Position, GameState — prototypes logique
│   ├── display.h        # Prototypes interface Raylib
│   ├── ipc.h            # Structure GameMessage (données du pipe)
│   └── score.h          # Prototypes gestion des scores
├── src/
│   ├── main.cpp         # Père : pipes, fork, boucle affichage, IPC
│   ├── game.cpp         # Logique : apply_move, is_trap_cell, random_token
│   ├── display.cpp      # Interface Raylib : grille, écrans accueil/fin/scores
│   └── score.cpp        # Lecture / écriture de score.txt
├── Makefile
├── test_ncurse.cpp      # Fichier de test ncurses (non inclus dans le binaire final)
├── score.txt            # Créé à l'exécution — historique des parties
└── README.md
```

### 4.2 Vue d'ensemble des processus

```
PROCESSUS PÈRE  (main)
  │
  │  Crée pipe12, pipe21, pipeParent
  │  Appelle fork() × 2, ferme les extrémités inutiles
  │  Lit pipeParent[0] en boucle -> display_grid() -> usleep(vitesse)
  │  waitpid(pid1) + waitpid(pid2) -> update_score_file()
  │
  ├─ fork() ──► FILS 1  (Joueur 1)
  │               srand(time(nullptr) ^ getpid())
  │               apply_move() -> write(pipe12) + write(pipeParent)
  │               Bloque sur read(pipe21) pour attendre J2
  │
  └─ fork() ──► FILS 2  (Joueur 2)
                  srand(time(nullptr) ^ getpid())
                  Bloque sur read(pipe12) pour attendre J1
                  apply_move() -> write(pipe21) + write(pipeParent)
```

### 4.3 Rôle de chaque entité

| Entité | Responsabilités principales |
|---|---|
| **Père** | Initialise Raylib, crée les 3 pipes, lance deux `fork()`, ferme les extrémités inutiles. Lit `pipeParent` en boucle pour afficher chaque coup. Appelle `waitpid()`, enregistre le score, propose de rejouer. |
| **Fils 1 (J1)** | Initialise `srand` avec `time ^ pid`. Tire `dx`/`dy`, appelle `apply_move()`, sérialise dans `GameMessage`, écrit dans `pipe12` et `pipeParent`, puis bloque sur `read(pipe21)`. |
| **Fils 2 (J2)** | Bloque sur `read(pipe12)`, désérialise l'état, tire `dx`/`dy`, appelle `apply_move()`, écrit dans `pipe21` et `pipeParent`. |

---

## 5. Description détaillée des fichiers

### `main.cpp` — Processus père et IPC

C'est le cœur du programme. Il contient la gestion complète des processus et des pipes, ainsi que les fonctions de sérialisation et désérialisation des messages.

| Fonction | Rôle |
|---|---|
| `state_to_message(state, player, dx, dy)` | Convertit un `GameState` en `GameMessage` pour l'envoi via pipe. |
| `message_to_state(msg)` | Reconstitue un `GameState` depuis un `GameMessage` reçu. |
| `write_message(fd, msg)` | Écrit exactement `sizeof(GameMessage)` octets dans le pipe. |
| `read_message(fd, msg)` | Lit exactement `sizeof(GameMessage)` octets depuis le pipe. |
| `player1_loop(…)` | Boucle du Fils 1 — joue, écrit dans `pipe12` et `pipeParent`, attend `pipe21`. |
| `player2_loop(…)` | Boucle du Fils 2 — attend `pipe12`, joue, écrit dans `pipe21` et `pipeParent`. |
| `main()` | Crée les pipes, forke, gère la boucle d'affichage père, appelle score et rejouer. |

---

### `game.h` / `game.cpp` — Logique du jeu

Ces fichiers contiennent toute la logique du jeu de d'Alembert, indépendamment de l'affichage.

Ces fichiers contiennent la logique du jeu.

Ils servent à gérer :

- la position de départ des joueurs
- la création de l’état initial
- les cases valides de la grille
- les cases pièges
- le tirage aléatoire
- le déplacement des joueurs
- les conditions de victoire

La fonction la plus importante est `apply_move()`.

**Détail de `apply_move()` :**

```
1. Calcule (newX, newY) = position actuelle + (dx, dy)
2. Si hors grille -> retourne false, pion immobile
3. Si case piégée -> pion en (1,1), retourne false (pas de victoire)
4. Déplacement effectif vers (newX, newY)
5. Si collision avec adversaire -> adversaire en (1,1)
6. Si (11,11) atteint -> finished=true, winner=player, retourne true
7. Si (1,1) atteint normalement -> finished=true, winner=player, retourne true
```

---

### `ipc.h` — Structure de communication inter-processus

Ce fichier définit uniquement la structure `GameMessage` utilisée pour tous les échanges via les pipes.

```cpp
struct GameMessage {
    int type;           // 0 = tour normal  |  1 = fin de partie
    int currentPlayer;  // Joueur qui vient de jouer (1 ou 2)
    int p1x, p1y;       // Position du Joueur 1 après le coup
    int p2x, p2y;       // Position du Joueur 2 après le coup
    int dx, dy;         // Jetons tirés ce tour
    int winner;         // 0 = aucun  |  1 = J1 gagne  |  2 = J2 gagne
};
```

> La taille fixe de la structure garantit que `write()` et `read()` transmettent exactement `sizeof(GameMessage)` octets à chaque appel, sans fragmentation ni ambiguïté.

---

### `display.h` / `display.cpp` — Interface graphique Raylib

Ces fichiers gèrent l'intégralité de l'interface graphique avec **Raylib**. La fenêtre est créée et contrôlée **uniquement par le processus père**, afin d'éviter tout conflit de rendu entre processus.

| Fonction | Rôle |
|---|---|
| `init_display()` | Initialise la fenêtre Raylib. |
| `close_display()` | Ferme la fenêtre proprement. |
| `display_is_open()` | Vérifie si la fenêtre est encore active. |
| `show_welcome_screen()` | Écran d'accueil — choix de la vitesse, démarrage ou quitter. |
| `display_turn_info(player, dx, dy)` | Enregistre les infos du dernier coup pour affichage. |
| `display_grid(state)` | Dessine la grille 11×11, les cases piégées, les pions J1 et J2. |
| `show_end_screen_and_wait(winner)` | Affiche le vainqueur et attend une action de l'utilisateur. |
| `show_score_history_screen()` | Lit `score.txt` et affiche les dernières parties. |

---

### `score.h` / `score.cpp` — Gestion des scores

Ces fichiers gèrent la persistance des résultats de chaque partie dans le fichier `score.txt`.

| Fonction | Rôle |
|---|---|
| `update_score_file(winner)` | Ouvre `score.txt` en mode ajout (`ios::app`) et écrit le résultat. |
| `show_score_file()` | Affiche le contenu de `score.txt` sur la sortie standard. |

Format d'une entrée dans `score.txt` :

```
Victoire Joueur 1 (Bleu)
Victoire Joueur 2 (Vert)
Victoire Joueur 1 (Bleu)
```
### `Makefile`

```makefile
CXX      = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -Iinclude -I/usr/local/include
LDFLAGS  = -L/usr/local/lib -lraylib -lm -lpthread -ldl -lrt -lX11

SRC      = src/main.cpp src/game.cpp src/display.cpp src/score.cpp
OBJ      = $(SRC:.cpp=.o)
TARGET   = dalembert

all: $(TARGET)

$(TARGET): $(OBJ)
    $(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ) $(LDFLAGS)

src/%.o: src/%.cpp
    $(CXX) $(CXXFLAGS) -c $< -o $@

clean:
    rm -f $(OBJ) $(TARGET) score.txt

re: clean all

.PHONY: all clean re
```

**Détail des flags :**

| Flag | Rôle |
|---|---|
| `-Wall -Wextra` | Active tous les avertissements du compilateur. |
| `-std=c++17` | Compile en C++17 (requis pour certaines syntaxes utilisées). |
| `-Iinclude` | Ajoute le dossier `include/` au chemin de recherche des headers. |
| `-I/usr/local/include` | Chemin vers les headers Raylib installés localement. |
| `-L/usr/local/lib` | Chemin vers les bibliothèques compilées de Raylib. |
| `-lraylib` | Liaison avec la bibliothèque Raylib. |
| `-lm -lpthread -ldl -lrt -lX11` | Dépendances système requises par Raylib sous Linux. |

---

## 6. Communication inter-processus

### 6.1 Les trois pipes

Trois pipes anonymes sont créés par le père **avant tout `fork()`**. Chacun est strictement unidirectionnel.

| Pipe | Direction | Contenu |
|---|---|---|
| `pipe12` | Fils 1 -> Fils 2 | `GameMessage` après chaque coup de J1 |
| `pipe21` | Fils 2 -> Fils 1 | `GameMessage` après chaque coup de J2 |
| `pipeParent` | Fils 1 & 2 -> Père | `GameMessage` envoyé après chaque coup — le père l'utilise pour afficher la grille |

### 6.2 Fermeture des descripteurs

Après les deux `fork()`, chaque processus ferme immédiatement les extrémités de pipe qu'il n'utilise pas. C'est une règle critique sous UNIX : un descripteur en écriture non fermé empêche le `read()` du lecteur de détecter la fin de flux (EOF), causant un blocage infini.

| Descripteur | Père | Fils 1 | Fils 2 |
|---|:---:|:---:|:---:|
| `pipe12[0]` — lecture | ferme | ferme | **garde** |
| `pipe12[1]` — écriture | ferme | **garde** | ferme |
| `pipe21[0]` — lecture | ferme | **garde** | ferme |
| `pipe21[1]` — écriture | ferme | ferme | **garde** |
| `pipeParent[0]` — lecture | **garde** | ferme | ferme |
| `pipeParent[1]` — écriture | ferme | **garde** | **garde** |

### 6.3 Synchronisation

La synchronisation entre les deux fils est assurée naturellement par le caractère bloquant de `read()` sur un pipe vide. Le séquencement exact est le suivant :

```
Fils 1  :  apply_move() -> write(pipeParent) + write(pipe12) -> bloque sur read(pipe21)
Père    :  débloqué -> display_grid() -> usleep(vitesse)
Fils 2  :  débloqué par read(pipe12) -> apply_move() -> write(pipeParent) + write(pipe21)
Père    :  débloqué -> display_grid() -> usleep(vitesse)
Fils 1  :  débloqué par read(pipe21) -> nouveau tour...
```

---

## 7. Interface graphique — Raylib

### 7.1 Pourquoi Raylib ?

La bibliothèque **Raylib** a été choisie pour les raisons suivantes :

- compatibilité native avec C++ sous Linux/Ubuntu sans configuration complexe
- légèreté et absence de dépendances externes lourdes
- primitives de dessin simples (`DrawRectangle`, `DrawCircle`, `DrawText`)
- intégration facile avec un projet compilé via `Makefile`

### 7.2 Affichage de la grille

- Grille dessinée case par case (11 colonnes × 11 lignes) avec bordures.
- **Cases piégées** affichées en rouge.
- **Joueur 1** représenté par un cercle bleu.
- **Joueur 2** représenté par un cercle vert.
- Coordonnées actuelles de chaque joueur affichées en bas de la fenêtre.
- Informations du dernier coup (joueur, `dx`, `dy`) visibles en permanence.

### 7.3 Écrans de navigation

| Écran | Déclencheur | Actions disponibles |
|---|---|---|
| **Accueil** | Lancement du programme | Choisir la vitesse (`1`/`2`/`3`), démarrer (`Entrée`/`Espace`/clic), quitter (`Q`/`Échap`) |
| **Jeu** | Pendant la partie | Changer la vitesse (`1`/`2`/`3`), fermer la fenêtre |
| **Fin de partie** | Victoire détectée | `R` = Rejouer, `H` = Historique des scores, `Q` = Quitter |
| **Historique** | Touche `H` sur l'écran de fin | Affiche les dernières parties enregistrées dans `score.txt`, `B`/`Échap` pour revenir |

---

## 8. Fonctionnalité de vitesse dynamique

### 8.1 Choix de la vitesse au lancement

L'écran d'accueil propose trois niveaux de vitesse avant de démarrer la partie :

| Touche | Niveau | Délai entre les coups |
|:---:|---|---|
| `1` | Lente | ~1 seconde |
| `2` | Normale | ~0,5 seconde |
| `3` | Rapide | ~0,1 seconde |

### 8.2 Changement de vitesse en temps réel

Pendant la partie, l'utilisateur peut modifier la vitesse à tout moment sans interrompre le jeu :

- **Touche `1`** -> vitesse lente
- **Touche `2`** -> vitesse normale
- **Touche `3`** -> vitesse rapide

La vitesse est appliquée via `usleep()` dans la boucle d'affichage du processus père :

```cpp
usleep(GAME_SPEED * 1'000'000);
```

La variable `GAME_SPEED` est mise à jour en temps réel selon la touche pressée, sans perturber la logique des processus fils.

---

## 9. Flux d'exécution

### 9.1 Démarrage

1. `main()` appelle `init_display()` -> initialise la fenêtre Raylib.
2. `show_welcome_screen()` → boucle d'événements, choix de la vitesse initiale.
3. Si l'utilisateur quitte à l'écran d'accueil -> `close_display()` et fin du programme.
4. Création des 3 pipes (`pipe12`, `pipe21`, `pipeParent`).
5. Premier `fork()` -> création du Fils 1 (`player1_loop`).
6. Deuxième `fork()` -> création du Fils 2 (`player2_loop`).
7. Fermeture des descripteurs inutiles par chaque processus.

### 9.2 Déroulement d'une partie

```
Père     : affiche la grille initiale -> sleep(1)
Fils 1   : joue -> write(pipe12 + pipeParent)
Père     : reçoit -> display_grid() -> usleep(vitesse)
Fils 2   : reçoit -> joue -> write(pipe21 + pipeParent)
Père     : reçoit -> display_grid() -> usleep(vitesse)
... (alternance jusqu'à victoire)
```

Quand `state.finished == true` :
- Le fils gagnant envoie un dernier message avec `type=1`.
- Les deux fils ferment leurs descripteurs et appellent `_exit(0)`.
- Le père sort de sa boucle de lecture.

### 9.3 Fin de partie et rejeu

1. `waitpid(pid1)` + `waitpid(pid2)` — attente de la terminaison propre des fils.
2. `update_score_file(winner)` — écriture dans `score.txt` en mode ajout.
3. `show_end_screen_and_wait(winner)` — affichage du vainqueur.
4. Si `R` -> nouvelle itération de la boucle `while(choice == 'r')` : nouveaux pipes, nouveaux `fork()`.
5. Si `Q` ou fermeture de fenêtre -> `close_display()` et `return 0`.

---

## 10. Prérequis et installation

### Système requis

- Ubuntu ( mais Arch Linux recommandé, compatible Ubuntu/Debian)
- Compilateur `g++` avec support C++17 (version ≥ 9)
- Bibliothèque **Raylib** installée dans `/usr/local/`

### Installation de Raylib (si nécessaire)

```bash
# Cloner et compiler Raylib depuis les sources
git clone https://github.com/raysan5/raylib.git
cd raylib/src
make PLATFORM=PLATFORM_DESKTOP
sudo make install
```

### Vérification de l'installation

```bash
# Vérifier que Raylib est bien installé
ls /usr/local/lib/libraylib*
ls /usr/local/include/raylib.h
```

### Dépendances système (Ubuntu / Debian)

```bash
sudo apt install g++ make libx11-dev
```

---

## 11. Tests réalisés

| Test | Résultat |
|---|---|
| Compilation sans erreur ni avertissement | ✓ |
| Lancement du programme et affichage de l'écran d'accueil | ✓ |
| Choix de vitesse au lancement (touches 1, 2, 3) | ✓ |
| Démarrage de la partie (clic bouton / Entrée / Espace) | ✓ |
| Alternance correcte des tours entre J1 et J2 | ✓ |
| Affichage graphique de la grille et des pions | ✓ |
| Déplacement valide — pion se déplace correctement | ✓ |
| Déplacement hors grille — pion immobile | ✓ |
| Arrivée sur case piégée — retour en (1,1) sans victoire | ✓ |
| Collision — adversaire renvoyé en (1,1) | ✓ |
| Victoire sur (11,11) détectée correctement | ✓ |
| Victoire sur (1,1) par tirage normal | ✓ |
| (1,1) atteint via case X — pas de victoire | ✓ |
| Changement de vitesse en cours de partie | ✓ |
| Création et écriture dans `score.txt` | ✓ |
| Affichage de l'historique des scores | ✓ |
| Rejouer sans relancer le programme | ✓ |
| Fermeture propre de la fenêtre | ✓ |
| Terminaison propre des fils via `waitpid()` | ✓ |

---

## 12. Quelques Difficultés rencontrées et solutions

### Vitesse dynamique sans perturber les fils

**Problème :** modifier la vitesse en temps réel depuis le père sans impacter le rythme interne des fils.

**Solution :** la variable `GAME_SPEED` est lue uniquement par le père dans son `usleep()`. Les fils ne sont pas concernés — leur rythme est naturellement régulé par le blocage sur `read()`.

### Gestion correcte de la condition de victoire sur (1,1)

**Problème :** distinguer un arrivée normale en (1,1) d'un retour forcé par une case X ou une collision.

**Solution :** dans `apply_move()`, le retour forcé (`is_trap_cell`) déclenche un `return false` avant même d'atteindre la vérification de victoire. Seul un déplacement effectif vers (1,1) peut déclencher `finished = true`.

---

## 13. Améliorations possibles

- Ajouter un **mode joueur humain** — permettre à un joueur de saisir manuellement `dx` et `dy`.
- Implémenter une **intelligence artificielle** avec stratégie (préférer les coups rapprochant des coins gagnants).
- Afficher des **statistiques de session** — taux de victoire, nombre moyen de tours par partie.
- Ajouter des **animations** de déplacement des pions (transition fluide entre cases).
- Exporter l'historique des scores au **format JSON ou CSV**.
- Ajouter un **menu de paramètres** pour personnaliser les couleurs des joueurs.
- Afficher un **compteur de tours** en temps réel pendant la partie.

---

## 14. Sources et outils utilisés

### Documentation officielle

- Sujet du projet INFO4108 — Professeur Éric Hervet, Hiver 2026
- Notes de cours INFO4108
- Documentation Raylib — https://www.raylib.com/
- Documentation C++17 — https://en.cppreference.com/
- Pages de manuel UNIX : `man 2 fork`, `man 2 pipe`, `man 2 read`, `man 2 write`, `man 2 waitpid`

### Références techniques

- Beej's Guide to Unix IPC — https://beej.us/guide/bgipc/
- cplusplus.com — https://cplusplus.com/

### Outils de développement

| Outil | Usage |
|---|---|
| `g++` | Compilation C++17 |
| `make` | Automatisation de la compilation |
| Visual Studio Code | Éditeur de code |
| Terminal Ubuntu | Compilation, tests, débogage |

### Langages et bibliothèques

| Technologie | Version | Rôle |
|---|---|---|
| C++ | C++17 | Langage principal |
| Raylib | 5.x | Interface graphique |
| UNIX (fork, pipe) | — | Gestion des processus et IPC |

### Intelligence artificielle

- **Claude (Anthropic, claude.ai)** — utilisé pour la compréhension de certaines notions système, la mise sur pied de l'architecture du projet et la correction d'erreurs.

---

## 15. Conclusion

Ce projet m'a permis de mettre en pratique plusieurs notions fondamentales de la programmation système Linux :

- la **création et la gestion de processus concurrents** via `fork()`
- la **communication inter-processus** via des pipes anonymes
- la **synchronisation** par blocage naturel de `read()`
- la **modularité** d'un projet C++ organisé en plusieurs fichiers sources
- la **gestion de fichiers** pour la persistance des données
- l'**intégration d'une bibliothèque graphique** (Raylib) dans un projet système

J'ai également pu constater l'importance d'une fermeture rigoureuse des descripteurs de pipes après `fork()`, ainsi que la nécessité de séparer clairement les responsabilités entre le processus père (affichage, supervision) et les processus fils (logique de jeu).

Ce projet a été techniquement enrichissant et formateur, tant sur le plan de la programmation système que sur celui de la structuration d'un projet C++ complet.
