#ifndef GAME_H
#define GAME_H

/* -----------------------------------------------------------------------------
   Structure Position
   ------------------
   Représente une position (x, y) sur la grille 11x11.
----------------------------------------------------------------------------- */
struct Position {
    int x;
    int y;
};

/* -----------------------------------------------------------------------------
   Structure GameState
   --------------------
   Représente l'état complet du jeu à un instant donné :
     - p1 : position du pion du joueur 1
     - p2 : position du pion du joueur 2
     - finished : indique si la partie est terminée
     - winner : 0 = aucun gagnant, 1 = joueur 1, 2 = joueur 2
----------------------------------------------------------------------------- */
struct GameState {
    Position p1;
    Position p2;
    bool finished;
    int winner; // 0 = aucun, 1 = joueur 1, 2 = joueur 2
};

/* -----------------------------------------------------------------------------
   get_start_position
   -------------------
   Retourne la position de départ des deux joueurs.
   Dans le jeu de d'Alembert, les deux pions commencent au centre de la grille.
----------------------------------------------------------------------------- */
Position get_start_position();

/* -----------------------------------------------------------------------------
   create_initial_state
   ---------------------
   Initialise un GameState avec :
     - les deux pions au centre
     - finished = false
     - winner = 0
----------------------------------------------------------------------------- */
GameState create_initial_state();

/* -----------------------------------------------------------------------------
   is_valid_position
   ------------------
   Vérifie si une position (x, y) est dans la grille 11x11.
----------------------------------------------------------------------------- */
bool is_valid_position(int x, int y);

/* -----------------------------------------------------------------------------
   is_trap_cell
   -------------
   Retourne true si la case (x, y) est une case piégée (marquée X dans le PDF).
   Si un joueur tombe dessus, il retourne à la case (1,1).
----------------------------------------------------------------------------- */
bool is_trap_cell(int x, int y);

/* -----------------------------------------------------------------------------
   random_token
   -------------
   Tire un jeton aléatoire parmi {-2, -1, 0, 1, 2}.
   Utilisé pour déterminer dx et dy à chaque tour.
----------------------------------------------------------------------------- */
int random_token();

/* -----------------------------------------------------------------------------
   apply_move
   -----------
   Applique le déplacement dx, dy au joueur donné.
   Gère :
     - les cases piégées
     - les collisions entre joueurs
     - les limites de la grille
     - la détection de victoire
   Retourne true si le mouvement a été appliqué, false sinon.
----------------------------------------------------------------------------- */
bool apply_move(GameState& state, int player, int dx, int dy);

#endif
