#include "Game.h"
#include <termios.h>
#include <unistd.h>

static struct termios originalTermios;

void enableRawMode() {
    tcgetattr(STDIN_FILENO, &originalTermios);
    struct termios raw = originalTermios;
    raw.c_lflag &= ~(ECHO | ICANON);   // no echo, read char-by-char
    raw.c_cc[VMIN]  = 0;               // non-blocking
    raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void disableRawMode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &originalTermios);
}

int main() {
    enableRawMode();
    atexit(disableRawMode);  // restore terminal on exit

    printf("\033[?25l");     // hide cursor
    printf("\033[2J");       // clear screen once

    Game game;
    game.run();

    printf("\033[?25h");     // restore cursor
    printf("\033[2J\033[H"); // clear on exit
    return 0;
}