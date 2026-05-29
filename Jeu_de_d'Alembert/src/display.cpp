#include "display.h"
#include "game.h"
#include "score.h"

#include <raylib.h>
#include <string>
#include <fstream>

extern float GAME_SPEED;

// -----------------------------------------------------------------------------
// Constantes d'affichage 
// -----------------------------------------------------------------------------
static const int CELL_SIZE = 50;
static const int GRID_SIZE = 11;
static const int OFFSET_X = 90;
static const int OFFSET_Y = 90;

// Dernières infos du tour
static int lastPlayer = 0;
static int lastDx = 0;
static int lastDy = 0;

// -----------------------------------------------------------------------------
// Initialisation de la fenêtre Raylib
// -----------------------------------------------------------------------------
void init_display() {
    InitWindow(780, 760, "Jeu de d'Alembert - Raylib");
    SetTargetFPS(60);
}

// -----------------------------------------------------------------------------
// Fermeture propre de la fenêtre
// -----------------------------------------------------------------------------
void close_display() {
    CloseWindow();
}

// -----------------------------------------------------------------------------
// Vérifie si la fenêtre est ouverte
// -----------------------------------------------------------------------------
bool display_is_open() {
    return !WindowShouldClose();
}

// -----------------------------------------------------------------------------
// Mise à jour des infos du tour (affichées en bas)
// -----------------------------------------------------------------------------
void display_turn_info(int player, int dx, int dy) {
    lastPlayer = player;
    lastDx = dx;
    lastDy = dy;
}

// -----------------------------------------------------------------------------
// Écran d'accueil
// -----------------------------------------------------------------------------
char show_welcome_screen() {

    while (!WindowShouldClose()) {

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("Bienvenue au jeu", 250, 180, 34, BLACK);
        DrawText("de d'Alembert", 245, 235, 30, DARKBLUE);

        // Bouton "Commencer"
        Rectangle btn = {250, 360, 280, 70};
        DrawRectangleRec(btn, BLUE);
        DrawRectangleLinesEx(btn, 3, BLACK);
        DrawText("Commencer le jeu", 285, 382, 24, WHITE);

        DrawText("Cliquez ou appuyez sur ENTREE", 200, 500, 20, BLACK);
        DrawText("Q ou Echap pour quitter", 250, 540, 20, BLACK);
        DrawText("1: Lent  2: Normal  3: Rapide", 220, 580, 20, BLACK);
        EndDrawing();

        // Gestion des événements
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))
            return 's';

        if (IsKeyPressed(KEY_Q) || IsKeyPressed(KEY_ESCAPE))
            return 'q';

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 m = GetMousePosition();
            if (CheckCollisionPointRec(m, btn))
                return 's';
        }
        // Ajout un paramètre de vitesse dans l'écran d'accueil
        if (IsKeyPressed(KEY_ONE)) return 'l'; // lent
        if (IsKeyPressed(KEY_TWO)) return 'n'; // normal
        if (IsKeyPressed(KEY_THREE)) return 'r'; // rapide
    }

    return 'q';
}

// -----------------------------------------------------------------------------
// Affichage de la grille + pions + pièges
// -----------------------------------------------------------------------------
void display_grid(const GameState& state) {

    BeginDrawing();
    ClearBackground(RAYWHITE);

    DrawText("Jeu de d'Alembert", 250, 20, 28, DARKBLUE);

    // Dernier coup
    if (lastPlayer != 0) {
        std::string info = "Dernier coup : Joueur " +
                           std::to_string(lastPlayer) +
                           " dx=" + std::to_string(lastDx) +
                           " dy=" + std::to_string(lastDy);

        DrawText(info.c_str(), 40, 670, 20, BLACK);
    }

    // Numéros en haut
    for (int x = 1; x <= GRID_SIZE; ++x) {
        DrawText(std::to_string(x).c_str(),
                 OFFSET_X + (x - 1) * CELL_SIZE + 17,
                 OFFSET_Y - 35,
                 16,
                 BLACK);
    }

    // Numéros à gauche
    for (int y = 1; y <= GRID_SIZE; ++y) {
        DrawText(std::to_string(y).c_str(),
                 OFFSET_X - 35,
                 OFFSET_Y + (y - 1) * CELL_SIZE + 13,
                 16,
                 BLACK);
    }

    // Grille + pièges
    for (int y = 1; y <= GRID_SIZE; ++y) {
        for (int x = 1; x <= GRID_SIZE; ++x) {

            Color cellColor = is_trap_cell(x, y) ? RED : WHITE;

            DrawRectangle(OFFSET_X + (x - 1) * CELL_SIZE,
                          OFFSET_Y + (y - 1) * CELL_SIZE,
                          CELL_SIZE - 2,
                          CELL_SIZE - 2,
                          cellColor);

            DrawRectangleLines(OFFSET_X + (x - 1) * CELL_SIZE,
                               OFFSET_Y + (y - 1) * CELL_SIZE,
                               CELL_SIZE - 2,
                               CELL_SIZE - 2,
                               BLACK);
        }
    }

    // Pions
    auto drawPawn = [&](int x, int y, Color color) {
        DrawCircle(OFFSET_X + (x - 1) * CELL_SIZE + 25,
                   OFFSET_Y + (y - 1) * CELL_SIZE + 25,
                   15,
                   color);
    };

    if (state.p1.x == state.p2.x && state.p1.y == state.p2.y) {
        drawPawn(state.p1.x, state.p1.y, PURPLE);
    } else {
        drawPawn(state.p1.x, state.p1.y, BLUE);
        drawPawn(state.p2.x, state.p2.y, GREEN);
    }

    EndDrawing();

    // Pause demandée après chaque coup pour mieux visualiser les déplacements
    WaitTime(0.5);
}

// -----------------------------------------------------------------------------
// Historique des scores
// -----------------------------------------------------------------------------
void show_score_history_screen() {

    while (!WindowShouldClose()) {

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("Historique des scores", 230, 60, 30, BLACK);

        std::ifstream file("score.txt");
        if (!file) {
            DrawText("Aucun score enregistre.", 250, 180, 22, BLACK);
        } else {
            std::string line;
            int y = 140;
            int count = 1;

            while (std::getline(file, line) && count <= 15) {
                DrawText((std::to_string(count) + ". " + line).c_str(),
                         160, y, 18, BLACK);
                y += 32;
                count++;
            }
        }

        DrawText("[B] Retour", 300, 690, 20, BLACK);

        EndDrawing();

        if (IsKeyPressed(KEY_B) || IsKeyPressed(KEY_ESCAPE))
            return;
    }
}

// -----------------------------------------------------------------------------
// Écran de fin de partie
// -----------------------------------------------------------------------------
char show_end_screen_and_wait(int winner) {

    while (!WindowShouldClose()) {

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("FIN DE LA PARTIE", 250, 210, 32, BLACK);

        if (winner == 1)
            DrawText("Gagnant : Joueur 1 (Bleu)", 220, 290, 26, BLUE);
        else if (winner == 2)
            DrawText("Gagnant : Joueur 2 (Vert)", 220, 290, 26, GREEN);

        DrawText("[R] Rejouer", 300, 380, 22, BLACK);
        DrawText("[H] Historique des scores", 230, 430, 22, BLACK);
        DrawText("[Q] Quitter", 305, 480, 22, BLACK);

        EndDrawing();

        if (IsKeyPressed(KEY_R))
            return 'r';

        if (IsKeyPressed(KEY_H))
            show_score_history_screen();

        if (IsKeyPressed(KEY_Q) || IsKeyPressed(KEY_ESCAPE))
            return 'q';
    }

    return 'q';
}
