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
    moveTo(4, 12); printf("W/S to move, Enter to select \n\n");
    fflush(stdout);
}

void Render::drawGame(const Map& map, const Player& player, const std::vector<Slime>& slimes, bool isAttacking, int attackX, int attackY){
    clearScreen();
    drawMap(map);
    drawAttack(isAttacking, attackX, attackY);
    for (const auto& slime : slimes) {
        drawEnemy(slime);
    }
    for (const auto& slime : slimes) {
        for (const auto& projectile : slime.particles) {
            drawProjectile(projectile);
        }
    }
    drawPlayer(player);
    drawHUD(map,player.hp, player.maxHp);
    fflush(stdout);
}

void Render::drawProjectile(const slimeProjectile& projectile){
    moveTo(projectile.x, projectile.y);
    putchar('o');
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

void Render::drawSavePrompt(){
    clearScreen();
    moveTo(4, 3); printf("Save progress before quitting?");
    moveTo(4, 5); printf("[Y] Yes   [N] No\n\n");
    fflush(stdout);
}

void Render::drawContinuePrompt(){
    clearScreen();
    moveTo(4, 3); printf("Save file found!");
    moveTo(4, 5); printf("Continue from where you left off?");
    moveTo(4, 7); printf("[Y] Yes   [N] No (start fresh)");
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
    if(player.isBlinking) return; // if taking damage - skip

    moveTo(player.x, player.y);
    int lx = player.getLastX();
    int ly = player.getLastY();

    if      (lx ==  1) putchar('>');
    else if (lx == -1) putchar('<');
    else if (ly == -1) putchar('^');
    else if (ly ==  1) putchar('v');
    else               putchar('P'); // hasn't moved yet
}



void Render::drawEnemy(const Enemy& slime){
    moveTo(slime.x, slime.y);
    putchar('E');
}

void Render::drawHUD(const Map& map, int hp, int maxHp){
    moveTo(0, map.getHeight() + 1);
    int hpClamped = std::max(0, std::min(hp, maxHp));
    int hearts = (hpClamped * 3) / maxHp;
    printf("HP: %s%s%s  [%d/%d]  [P]Pause [Q]Quit [SPC]Atk",
        hearts >= 1 ? "<3" : "< ",
        hearts >= 2 ? "<3" : "< ",
        hearts >= 3 ? "<3" : "< ",
        hp, maxHp);
}

void Render::drawAttack(bool isAttacking, int attackX, int attackY){
    if(isAttacking){
        moveTo(attackX, attackY);
        putchar('*');
    }
}

void Render::drawTile(int x, int y, char tile){
    moveTo(x, y);
    if(tile == '|' || tile == '_'){
        printf("\033[1m%c\033[0m", tile); // bold walls
    } else {
        putchar(tile);
    }
}