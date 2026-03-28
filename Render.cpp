#include "Render.h"
#include <ncurses.h>

void Render::drawMenu(int menuSelection){
    clear();
    int cy = 3, cx = 4;
    mvprintw(cy,   cx, "  THE LEGEND OF HKU");
    mvprintw(cy+2, cx, "%s Start", menuSelection == 0 ? ">" : " ");
    mvprintw(cy+3, cx, "%s Quit",  menuSelection == 1 ? ">" : " ");
    mvprintw(cy+5, cx, "W/S to move, Enter to select");
    refresh();
}

void Render::drawGame(const Map& map, const Player& player, bool isAttacking, int attackX, int attackY){
    clear();
    drawMap(map);
    drawAttack(isAttacking, attackX, attackY);
    drawPlayer(player);
    drawHUD(map);
    refresh();
}

void Render::drawPauseOverlay(){
    clear();
    mvprintw(2, 4, "*** PAUSED ***");
    mvprintw(3, 4, "[P] Resume   [Q] Quit");
    refresh();
}

void Render::drawGameOver(){
    clear();
    mvprintw(3, 4, "  GAME OVER");
    mvprintw(5, 4, "Press ENTER to return to menu");
    refresh();
}

void Render::drawWin(){
    clear();
    mvprintw(3, 4, "  YOU WIN!");
    mvprintw(5, 4, "Press ENTER to return to menu");
    refresh();
}

// ===============
//     PRIVATE
// ===============

void Render::drawMap(const Map& map){
    for(int y = 0; y < map.getHeight(); y++){
        for(int x = 0; x < map.getWidth(); x++){
            drawTile(x, y, map.getTile(x, y));
        }      
    }      
}

void Render::drawPlayer(const Player& player){
    mvaddch((int)player.y, (int)player.x, 'P');
}

void Render::drawAttack(bool isAttacking, int attackX, int attackY){
    if(isAttacking) mvaddch(attackY, attackX, '*');
}

void Render::drawHUD(const Map& map){
    mvprintw(map.getHeight() + 1, 0, "[P] Pause  [Q] Quit  [SPACE] Attack");
}

void Render::drawTile(int x, int y, char tile){
    mvaddch(y, x, tile);
}