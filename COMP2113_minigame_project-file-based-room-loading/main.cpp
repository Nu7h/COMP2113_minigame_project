#include "Game.h"
#include <termios.h>
#include <unistd.h>

static struct termios originalTermios;

void enableRawMode() {
    tcgetattr(STDIN_FILENO, &originalTermios);
    struct termios raw = originalTermios;
    raw.c_lflag &= ~(ECHO | ICANON);   
    raw.c_cc[VMIN]  = 0;
    raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void disableRawMode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &originalTermios);
}

int main() {
    enableRawMode();
    atexit(disableRawMode); 

    printf("\033[?25l");     
    printf("\033[2J");       

    Game game;
    game.run();

    printf("\033[?25h");     
    printf("\033[2J\033[H"); 
    return 0;
}