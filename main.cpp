#include "Game.h"
#include <ncurses.h>

int main() {
    initscr();
    noecho();
    curs_set(0);
    nodelay(stdscr, TRUE);

    Game game;
    game.run();

    endwin();
    return 0;
}