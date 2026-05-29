#include <ncurses.h>

int main() {
    initscr();
    cbreak();
    noecho();
    curs_set(0);

    mvprintw(5, 10, "Bonjour ncurses !");
    mvprintw(7, 10, "Appuie sur une touche...");
    refresh();
    getch();

    endwin();
    return 0;
}