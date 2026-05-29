#ifndef SCORE_H
#define SCORE_H

/* -----------------------------------------------------------------------------
   update_score_file
   ------------------
   Enregistre dans le fichier score.txt le gagnant d'une partie.
   Le fichier est ouvert en mode append, ce qui permet de conserver
   l'historique complet des victoires.
----------------------------------------------------------------------------- */
void update_score_file(int winner);

/* -----------------------------------------------------------------------------
   show_score_file
   ----------------
   Affiche le contenu du fichier score.txt dans le terminal.
   Utile pour un affichage texte ou pour du debugging.
----------------------------------------------------------------------------- */
void show_score_file();

#endif
