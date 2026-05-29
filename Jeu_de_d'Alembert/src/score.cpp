#include "score.h"
#include <fstream>
#include <iostream>
#include <string>

using namespace std;

/* -----------------------------------------------------------------------------
   get_player_name
   ----------------
   Retourne le nom lisible du joueur gagnant.
   Cette fonction permet d'éviter de répéter du texte dans le code.
----------------------------------------------------------------------------- */
static string get_player_name(int winner) {
    if (winner == 1) return "Joueur 1 (Bleu)";
    if (winner == 2) return "Joueur 2 (Vert)";
    return "Inconnu";
}

/* -----------------------------------------------------------------------------
   update_score_file
   ------------------
   Ajoute une ligne dans le fichier score.txt pour enregistrer la victoire
   d'un joueur. Le fichier est ouvert en mode append (ajout), ce qui permet
   de conserver l'historique complet des parties.

   Paramètre :
     - winner : numéro du joueur gagnant (1 ou 2)
----------------------------------------------------------------------------- */
void update_score_file(int winner) {
    ofstream file("score.txt", ios::app);

    if (!file) {
        cerr << "Erreur ouverture score.txt\n";
        return;
    }

    file << "Victoire " << get_player_name(winner) << "\n";
}

/* -----------------------------------------------------------------------------
   show_score_file
   ----------------
   Affiche le contenu du fichier score.txt dans le terminal.
   Cette fonction n'est pas utilisée par Raylib, mais peut servir pour
   un affichage texte ou pour du debugging.
----------------------------------------------------------------------------- */
void show_score_file() {
    ifstream file("score.txt");

    cout << "\n===== SCORE =====\n";

    if (!file) {
        cout << "Aucun score enregistre.\n";
        cout << "=================\n";
        return;
    }

    string line;

    while (getline(file, line)) {
        cout << line << "\n";
    }

    cout << "=================\n";
}
