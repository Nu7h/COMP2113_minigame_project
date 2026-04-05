#include "Render.h"

static void moveTo(int x, int y) {
    
    printf("\033[%d;%dH", y + 1, x + 1);
}

static void clearScreen() {
    printf("\033[2J\033[H");
}

void Render::drawMenu(int menuSelection){
    clearScreen();
    moveTo(4, 3); printf("  THE LEGEND OF HKU");
    moveTo(4, 5); printf("  Select difficulty");
    moveTo(4, 7); printf("%s EASY",     menuSelection == 0 ? ">" : " ");
    moveTo(4, 8); printf("%s NORMAL",   menuSelection == 1 ? ">" : " ");
    moveTo(4, 9); printf("%s HARD",     menuSelection == 2 ? ">" : " ");
    moveTo(4, 10); printf("%s Quit",    menuSelection == 3 ? ">" : " ");
    moveTo(4, 12); printf("W/S to move, Enter to select");
    fflush(stdout);
}

void Render::drawGame(const Map& map, const Player& player, const Enemy& enemy, bool isAttacking, int attackX, int attackY){
    clearScreen();
    drawMap(map);
    drawAttack(isAttacking, attackX, attackY);
    drawPlayer(player);
    drawEnemy(enemy);
    drawHUD(map);
    fflush(stdout);
}

void Render::drawPauseOverlay(){
    clearScreen();
    moveTo(4, 2); printf("*** PAUSED ***");
    moveTo(4, 3); printf("[P] Resume   [Q] Quit");
    fflush(stdout);
}

void Render::drawGameOver(){
    clearScreen();
    moveTo(4, 3); printf("  GAME OVER");
    moveTo(4, 5); printf("Press ENTER to return to menu");
    fflush(stdout);
}

void Render::drawWin(){
    clearScreen();
    moveTo(4, 3); printf("  YOU WIN!");
    moveTo(4, 5); printf("Press ENTER to return to menu");
    fflush(stdout);
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
    moveTo(player.x, player.y);
    putchar('P');
}

void Render::drawAttack(bool isAttacking, int attackX, int attackY){
    if(isAttacking){
        moveTo(attackX, attackY);
        putchar('*');
    }
}

void Render::drawEnemy(const Enemy& slime){
    moveTo(slime.x, slime.y);
    putchar('E');
}

void Render::drawHUD(const Map& map){
    moveTo(0, map.getHeight() + 1);
    printf("[P] Pause  [Q] Quit  [SPACE] Attack");
}

void Render::drawTile(int x, int y, char tile){
    moveTo(x, y);
    putchar(tile);
}