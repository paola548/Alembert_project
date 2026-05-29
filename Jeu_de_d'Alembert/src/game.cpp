#include "game.h"
#include <cstdlib>

/* -----------------------------------------------------------------------------
   get_start_position
   -------------------
   Retourne la position de départ des deux joueurs.
   Dans le jeu de d'Alembert, les deux pions commencent au centre (6,6).
----------------------------------------------------------------------------- */
Position get_start_position() {
    return {6, 6};
}

/* -----------------------------------------------------------------------------
   create_initial_state
   ---------------------
   Initialise l'état du jeu :
     - pions au centre
     - partie non terminée
     - aucun gagnant
----------------------------------------------------------------------------- */
GameState create_initial_state() {
    GameState state;
    state.p1 = get_start_position();
    state.p2 = get_start_position();
    state.finished = false;
    state.winner = 0;
    return state;
}

/* -----------------------------------------------------------------------------
   is_valid_position
   ------------------
   Vérifie si une position (x,y) est dans la grille 11x11.
----------------------------------------------------------------------------- */
bool is_valid_position(int x, int y) {
    return x >= 1 && x <= 11 && y >= 1 && y <= 11;
}

/* -----------------------------------------------------------------------------
   is_trap_cell
   -------------
   Retourne true si la case (x,y) est une case piégée (marquée X dans le PDF).
   Configuration fixe selon la figure fournie dans l'énoncé.
----------------------------------------------------------------------------- */
bool is_trap_cell(int x, int y) {
    return (x == 3 && y == 3) ||
           (x == 3 && y == 9) ||
           (x == 5 && y == 5) ||
           (x == 7 && y == 7) ||
           (x == 9 && y == 3) ||
           (x == 9 && y == 9);
}

/* -----------------------------------------------------------------------------
   random_token
   -------------
   Tire un jeton aléatoire parmi {-2, -1, 0, 1, 2}.
----------------------------------------------------------------------------- */
int random_token() {
    return (std::rand() % 5) - 2; // [-2, 2]
}

/* -----------------------------------------------------------------------------
   apply_move
   -----------
   Applique le déplacement dx, dy au joueur donné.
   Gère :
     - les limites de la grille
     - les cases piégées
     - les collisions entre joueurs
     - la détection de victoire

   Retourne true si le mouvement entraîne une victoire.
----------------------------------------------------------------------------- */
bool apply_move(GameState& state, int player, int dx, int dy) {

    // Sélection du pion courant et du pion adverse
    Position* current = (player == 1) ? &state.p1 : &state.p2;
    Position* other   = (player == 1) ? &state.p2 : &state.p1;

    int oldX = current->x;
    int oldY = current->y;

    int newX = oldX + dx;
    int newY = oldY + dy;

    // 1) Si la case n'existe pas, le pion ne bouge pas
    if (!is_valid_position(newX, newY)) {
        return false;
    }

    // 2) Si la case est un piège, retour à (1,1)
    if (is_trap_cell(newX, newY)) {
        current->x = 1;
        current->y = 1;
        return false; // pas de victoire possible via un piège
    }

    // 3) Déplacement normal
    current->x = newX;
    current->y = newY;

    // 4) Collision : si on tombe sur l'adversaire, il retourne à (1,1)
    if (current->x == other->x && current->y == other->y) {
        other->x = 1;
        other->y = 1;
    }

    // 5) Victoire sur (11,11)
    if (current->x == 11 && current->y == 11) {
        state.finished = true;
        state.winner = player;
        return true;
    }

    // 6) Victoire sur (1,1) uniquement via un déplacement normal
    if (current->x == 1 && current->y == 1) {
        state.finished = true;
        state.winner = player;
        return true;
    }

    return false;
}
