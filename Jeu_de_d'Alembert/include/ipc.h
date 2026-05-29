#ifndef IPC_H
#define IPC_H

/* -----------------------------------------------------------------------------
   Structure GameMessage
   ----------------------
   Cette structure représente le message échangé entre les deux processus fils
   (joueur 1 et joueur 2) ainsi qu'entre les fils et le processus père.

   Elle contient :
     - type : indique si la partie continue (0) ou si elle est terminée (1)
     - currentPlayer : joueur qui vient de jouer (1 ou 2)
     - p1x, p1y : position du pion du joueur 1
     - p2x, p2y : position du pion du joueur 2
     - dx, dy : jetons tirés par le joueur (déplacement)
     - winner : 0 si personne n'a gagné, sinon 1 ou 2

   Cette structure est envoyée telle quelle via write() et read() dans les pipes.
----------------------------------------------------------------------------- */
struct GameMessage {
    int type;          // 0 = tour normal, 1 = fin de partie
    int currentPlayer; // joueur qui vient de jouer
    int p1x;           // position X du joueur 1
    int p1y;           // position Y du joueur 1
    int p2x;           // position X du joueur 2
    int p2y;           // position Y du joueur 2
    int dx;            // jeton tiré pour l'abscisse
    int dy;            // jeton tiré pour l'ordonnée
    int winner;        // 0 = aucun, 1 = joueur 1, 2 = joueur 2
};

#endif
