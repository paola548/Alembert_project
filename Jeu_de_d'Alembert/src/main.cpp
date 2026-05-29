#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>
#include <ctime>
#include <raylib.h>

#include "game.h"
#include "display.h"
#include "ipc.h"
#include "score.h"

float GAME_SPEED = 0.5f;

/* ---------------------------------------------------------------------------
   Fonctions utilitaires pour écrire/lire un message structuré dans un pipe.
   Elles garantissent que la totalité de la structure GameMessage est transmise.
--------------------------------------------------------------------------- */
static bool write_message(int fd, const GameMessage& msg) {
    ssize_t n = write(fd, &msg, sizeof(msg));
    return n == (ssize_t)sizeof(msg);
}

static bool read_message(int fd, GameMessage& msg) {
    ssize_t n = read(fd, &msg, sizeof(msg));
    return n == (ssize_t)sizeof(msg);
}

/* ---------------------------------------------------------------------------
   Conversion GameState -> GameMessage pour envoyer l'état du jeu via pipe.
--------------------------------------------------------------------------- */
static GameMessage state_to_message(
    const GameState& state,
    int currentPlayer,
    int dx,
    int dy
) {
    GameMessage msg;

    msg.type = state.finished ? 1 : 0;
    msg.currentPlayer = currentPlayer;

    msg.p1x = state.p1.x;
    msg.p1y = state.p1.y;

    msg.p2x = state.p2.x;
    msg.p2y = state.p2.y;

    msg.dx = dx;
    msg.dy = dy;

    msg.winner = state.winner;

    return msg;
}

/* ---------------------------------------------------------------------------
   Conversion GameMessage -> GameState pour reconstruire l'état reçu.
--------------------------------------------------------------------------- */
static GameState message_to_state(const GameMessage& msg) {
    GameState state;

    state.p1.x = msg.p1x;
    state.p1.y = msg.p1y;

    state.p2.x = msg.p2x;
    state.p2.y = msg.p2y;

    state.finished = (msg.type == 1);
    state.winner = msg.winner;

    return state;
}

/* ===========================================================================
   PROCESSUS FILS : JOUEUR 1
   - Tire dx, dy
   - Applique le coup
   - Envoie l'état au père et au joueur 2
   - Attend la réponse du joueur 2
=========================================================================== */
static void player1_loop(
    int read_from_p2,
    int write_to_p2,
    int write_to_parent
) {
    std::srand((unsigned int)(std::time(nullptr) ^ getpid()));

    GameState state = create_initial_state();

    while (!state.finished) {

        // Tirage aléatoire des jetons
        int dx = random_token();
        int dy = random_token();

        // Application du coup
        apply_move(state, 1, dx, dy);

        // Construction du message
        GameMessage msg = state_to_message(state, 1, dx, dy);

        // Envoi au père et au joueur 2
        write_message(write_to_parent, msg);
        write_message(write_to_p2, msg);

        if (state.finished)
            break;

        // Attente du coup du joueur 2
        if (!read_message(read_from_p2, msg))
            break;

        state = message_to_state(msg);
    }

    // Fermeture des pipes
    close(read_from_p2);
    close(write_to_p2);
    close(write_to_parent);

    _exit(0);
}

/* ===========================================================================
   PROCESSUS FILS : JOUEUR 2
   - Attend le coup du joueur 1
   - Tire dx, dy
   - Applique le coup
   - Envoie l'état au père et au joueur 1
=========================================================================== */
static void player2_loop(
    int read_from_p1,
    int write_to_p1,
    int write_to_parent
) {
    std::srand((unsigned int)(std::time(nullptr) ^ getpid()));

    GameState state = create_initial_state();

    while (true) {
        GameMessage msg;

        // Attend le message du joueur 1
        if (!read_message(read_from_p1, msg))
            break;

        state = message_to_state(msg);

        if (state.finished)
            break;

        // Tirage aléatoire
        int dx = random_token();
        int dy = random_token();

        apply_move(state, 2, dx, dy);

        msg = state_to_message(state, 2, dx, dy);

        // Envoi au père et au joueur 1
        write_message(write_to_parent, msg);
        write_message(write_to_p1, msg);

        if (state.finished)
            break;
    }

    close(read_from_p1);
    close(write_to_p1);
    close(write_to_parent);

    _exit(0);
}

/* ===========================================================================
   PROCESSUS PÈRE (MAIN)
   - Affiche le menu
   - Crée les pipes
   - Lance les deux fils
   - Affiche la grille après chaque coup
   - Attend la fin de la partie
   - Enregistre le score
   - Propose de rejouer
=========================================================================== */
int main() {
    init_display();

    char startChoice = show_welcome_screen();
    if (startChoice != 's' && startChoice != 'l' && startChoice != 'n' && startChoice != 'r') {
        close_display();
        return 0;
    }

    if (startChoice == 'l') GAME_SPEED = 1.0f;   // lent
    if (startChoice == 'n') GAME_SPEED = 0.5f;   // normal
    if (startChoice == 'r') GAME_SPEED = 0.2f;   // rapide

    char choice = 'r';

    while (choice == 'r' || choice == 'R') {

        // Pipes :
        // pipe12 : joueur1 -> joueur2
        // pipe21 : joueur2 -> joueur1
        // pipeParent : fils -> père
        int pipe12[2];
        int pipe21[2];
        int pipeParent[2];

        if (pipe(pipe12) == -1 ||
            pipe(pipe21) == -1 ||
            pipe(pipeParent) == -1) {
            std::cerr << "Erreur creation des pipes.\n";
            close_display();
            return 1;
        }

        // Fork joueur 1
        pid_t pid1 = fork();
        if (pid1 < 0) {
            std::cerr << "Erreur fork joueur 1.\n";
            close_display();
            return 1;
        }

        if (pid1 == 0) {
            // Fils 1 : ferme les extrémités inutiles
            close(pipe12[0]);
            close(pipe21[1]);
            close(pipeParent[0]);

            player1_loop(
                pipe21[0],   // lit joueur2
                pipe12[1],   // écrit vers joueur2
                pipeParent[1] // écrit vers père
            );
        }

        // Fork joueur 2
        pid_t pid2 = fork();
        if (pid2 < 0) {
            std::cerr << "Erreur fork joueur 2.\n";
            close_display();
            return 1;
        }

        if (pid2 == 0) {
            close(pipe12[1]);
            close(pipe21[0]);
            close(pipeParent[0]);

            player2_loop(
                pipe12[0],   // lit joueur1
                pipe21[1],   // écrit vers joueur1
                pipeParent[1] // écrit vers père
            );
        }

        // Père : ferme les extrémités inutiles
        close(pipe12[0]);
        close(pipe12[1]);
        close(pipe21[0]);
        close(pipe21[1]);
        close(pipeParent[1]);

        // Affichage initial
        GameState initialState = create_initial_state();
        display_grid(initialState);
        sleep(1);

        GameMessage msg;
        int winner = 0;

        // Boucle d'affichage des coups
        while (display_is_open()) {

            if (!read_message(pipeParent[0], msg))
                break;

            GameState state = message_to_state(msg);

            // --- Changement de vitesse en direct ---
            if (IsKeyPressed(KEY_ONE))   GAME_SPEED = 1.0f;   // lent
            if (IsKeyPressed(KEY_TWO))   GAME_SPEED = 0.5f;   // normal
            if (IsKeyPressed(KEY_THREE)) GAME_SPEED = 0.2f;   // rapide
            
            // Affichage du coup
            display_turn_info(msg.currentPlayer, msg.dx, msg.dy);
            display_grid(state);

            // Pause selon la vitesse actuelle
            usleep(GAME_SPEED * 1000000);

            if (state.finished) {
                winner = state.winner;
                break;
            }
        }

        close(pipeParent[0]);

        // Attendre la fin des fils
        waitpid(pid1, nullptr, 0);
        waitpid(pid2, nullptr, 0);

        // Fin de partie
        if (winner != 0) {
            update_score_file(winner);
            choice = show_end_screen_and_wait(winner);
        } else {
            choice = 'q';
        }
    }

    close_display();
    return 0;
}
