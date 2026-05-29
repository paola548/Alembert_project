#ifndef DISPLAY_H
#define DISPLAY_H

#include "game.h"

/* -----------------------------------------------------------------------------
   init_display
   -------------
   Initialise la fenêtre graphique (Raylib) et prépare l'affichage du jeu.
----------------------------------------------------------------------------- */
void init_display();

/* -----------------------------------------------------------------------------
   close_display
   --------------
   Ferme proprement la fenêtre graphique et libère les ressources associées.
----------------------------------------------------------------------------- */
void close_display();

/* -----------------------------------------------------------------------------
   display_is_open
   ----------------
   Retourne true tant que la fenêtre Raylib est ouverte.
   Utilisé par le processus père pour savoir s'il doit continuer l'affichage.
----------------------------------------------------------------------------- */
bool display_is_open();

/* -----------------------------------------------------------------------------
   show_welcome_screen
   --------------------
   Affiche l'écran d'accueil du jeu.
   Retourne :
     - 's' pour commencer une partie
     - 'q' pour quitter
----------------------------------------------------------------------------- */
char show_welcome_screen();

/* -----------------------------------------------------------------------------
   display_turn_info
   ------------------
   Enregistre les informations du dernier coup joué (joueur, dx, dy).
   Ces informations seront affichées dans display_grid().
----------------------------------------------------------------------------- */
void display_turn_info(int player, int dx, int dy);

/* -----------------------------------------------------------------------------
   display_grid
   -------------
   Affiche la grille 11x11, les pièges, les pions et les informations du tour.
   Appelée après chaque coup reçu par le processus père.
----------------------------------------------------------------------------- */
void display_grid(const GameState& state);

/* -----------------------------------------------------------------------------
   show_end_screen_and_wait
   -------------------------
   Affiche l'écran de fin de partie avec le gagnant.
   Permet de choisir :
     - 'r' pour rejouer
     - 'q' pour quitter
     - 'h' pour afficher l'historique des scores
----------------------------------------------------------------------------- */
char show_end_screen_and_wait(int winner);

/* -----------------------------------------------------------------------------
   show_score_history_screen
   --------------------------
   Affiche l'historique des scores enregistrés dans score.txt.
----------------------------------------------------------------------------- */
void show_score_history_screen();

#endif
