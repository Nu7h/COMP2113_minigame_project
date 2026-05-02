#include "Render.h"

//define colors
#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_BRED    "\033[91m"
#define COLOR_BGREEN  "\033[92m"
#define COLOR_BYELLOW "\033[93m"
#define COLOR_BWHITE  "\033[97m"
#define COLOR_BOLD    "\033[1m"

static void moveTo(int x, int y) {
    printf("\033[%d;%dH", y + 1, x + 1);
}

static void clearScreen() {
    printf("\033[2J\033[H");
}

void Render::drawStartMenu(int menuSelection, bool hasSaveFile) {
    clearScreen();

    moveTo(4, 2);  printf(COLOR_BYELLOW COLOR_BOLD " THE LEGEND OF HKU" COLOR_RESET);
    moveTo(4, 4);  printf(COLOR_BWHITE " Main Menu" COLOR_RESET);

    moveTo(4, 6);  printf("%s " COLOR_BGREEN "New" COLOR_RESET,
                          menuSelection == 0 ? ">" : " ");

    if (hasSaveFile) {
        moveTo(4, 7);  printf("%s " COLOR_BYELLOW "Continue" COLOR_RESET,
                              menuSelection == 1 ? ">" : " ");
    } else {
        moveTo(4, 7);  printf("%s Continue " COLOR_RESET COLOR_RED "(no save found)" COLOR_RESET,
                              menuSelection == 1 ? ">" : " ");
    }

    moveTo(4, 8);  printf("%s Quit", menuSelection == 2 ? ">" : " ");

    moveTo(4, 10); printf(COLOR_CYAN "W/S" COLOR_RESET " to move, "
                          COLOR_CYAN "Enter" COLOR_RESET " to select");
    fflush(stdout);
}

// Renders difficulty selection menu
void Render::drawDifficultyMenu(int menuSelection) {
    clearScreen();

    moveTo(4, 2);  printf(COLOR_BYELLOW COLOR_BOLD " THE LEGEND OF HKU" COLOR_RESET);
    moveTo(4, 4);  printf(COLOR_BWHITE " Difficulty" COLOR_RESET);

    moveTo(4, 6);  printf("%s " COLOR_BGREEN "Easy" COLOR_RESET,
                          menuSelection == 0 ? ">" : " ");
    moveTo(4, 7);  printf("%s " COLOR_BYELLOW "Medium" COLOR_RESET,
                          menuSelection == 1 ? ">" : " ");
    moveTo(4, 8);  printf("%s " COLOR_BRED "Hard" COLOR_RESET,
                          menuSelection == 2 ? ">" : " ");
    moveTo(4, 9);  printf("%s " COLOR_CYAN    "Explore" COLOR_RESET, 
                          menuSelection == 3 ? ">" : " ");
    moveTo(4, 10); printf("%s Back",                                  
                          menuSelection == 4 ? ">" : " ");
    moveTo(4, 12); printf(COLOR_CYAN "W/S" COLOR_RESET " to move, "
                          COLOR_CYAN "Enter" COLOR_RESET " to select");
    fflush(stdout);
}

// Renders full game screen (map, enemies, boss, UI, etc.)
void Render::drawGame(const Map& map, const Player& player, const std::vector<Slime>& slimes, class Boss* boss, bool isAttacking, int attackX, int attackY,  bool roomLocked, const std::vector<struct Heart>& hearts){
    clearScreen();
    drawMap(map);
    drawLockedDoors(map, roomLocked);
    for (const auto& heart : hearts) {
        drawHeart(heart);
    }
    for (const auto& slime : slimes) {
        drawEnemy(slime);
    }
    for (const auto& slime : slimes) {
        for (const auto& projectile : slime.particles) {
            drawProjectile(projectile);
        }
    }
    if (boss != nullptr) {
        drawBoss(*boss);
        for (const auto& proj : boss->bossProjectiles) {
            drawBossProjectile(proj);
        }
    }
    drawAttack(isAttacking, attackX, attackY, player.getLastX());
    drawPlayer(player, map);
    drawHUD(map, player.hp, player.maxHp, boss, roomLocked, slimes);
    fflush(stdout);
}

// Renders slime projectile
void Render::drawProjectile(const slimeProjectile& projectile){
    moveTo(projectile.x, projectile.y+1);
    printf(COLOR_BRED COLOR_BOLD "o" COLOR_RESET);
}

// Renders boss projectile
void Render::drawBossProjectile(const struct bossProjectile& projectile){
    moveTo(projectile.x, projectile.y+1);
    printf(COLOR_BRED COLOR_BOLD "*" COLOR_RESET);
}

// Render a health heart on the map at the given position
// Hearts are displayed in red and drop from slimes when they are defeated
// Player heals 10 HP by walking over a heart
void Render::drawHeart(const Heart& heart) {
    moveTo(heart.x, heart.y+1);
    printf(COLOR_RED COLOR_BOLD "♥" COLOR_RESET);
}

void Render::drawPauseOverlay(){
    clearScreen();
    moveTo(4, 2); printf(COLOR_BYELLOW COLOR_BOLD "*** PAUSED ***" COLOR_RESET);
    moveTo(4, 3); printf(COLOR_CYAN "[P]" COLOR_RESET " Resume   " COLOR_CYAN "[Q]" COLOR_RESET " Quit");
    fflush(stdout);
}

void Render::drawGameOver(){
    clearScreen();
    moveTo(4, 3); printf(COLOR_BRED COLOR_BOLD "  GAME OVER" COLOR_RESET);
    moveTo(4, 5); printf("Press " COLOR_CYAN "ENTER" COLOR_RESET " to return to menu");
    fflush(stdout);
}

void Render::drawWin(){
    clearScreen();
    moveTo(4, 3); printf(COLOR_BGREEN COLOR_BOLD "  YOU WIN!" COLOR_RESET);
    moveTo(4, 5); printf("Press " COLOR_CYAN "ENTER" COLOR_RESET " to return to menu");
    fflush(stdout);
}

void Render::drawSavePrompt(){
    clearScreen();
    moveTo(4, 3); printf(COLOR_BYELLOW "Save progress before quitting?" COLOR_RESET);
    moveTo(4, 5); printf(COLOR_BGREEN "[Y] Yes" COLOR_RESET "   " COLOR_BRED "[N] No" COLOR_RESET "\n\n");
    fflush(stdout);
}


// ===============
//     PRIVATE
// ===============

// Renders map grid and room name
void Render::drawMap(const Map& map){
    std::string name = map.getRoomName();
    int mapWidth = map.getWidth();

    int centerX = (mapWidth - name.length() - 4) / 2;
    if (centerX < 0) centerX = 0;

    moveTo(centerX, 0);
    printf("[ %s ]", name.c_str());
    for(int y = 0; y < map.getHeight(); y++){
        for(int x = 0; x < map.getWidth(); x++){
            drawTile(x, y+1, map.getTile(x, y));
        }      
    }

}

// Renders player character and shield
void Render::drawPlayer(const Player& player, const Map& map){
    if(player.isBlinking) return;

    moveTo(player.x, player.y+1);
    int lx = player.getLastX();
    int ly = player.getLastY();

    char symbol = 'P';
    if      (lx ==  1) symbol = '>';
    else if (lx == -1) symbol = '<';
    else if (ly == -1) symbol = '^';
    else if (ly ==  1) symbol = 'v';

    printf(COLOR_BGREEN COLOR_BOLD "%c" COLOR_RESET, symbol);

    if(player.isShielding){

        int sx = player.x + player.getLastX();
        int sy = player.y+1 + player.getLastY();
        if(map.isWalkable(sx, sy-1)){
        moveTo(sx, sy);
        if(ly != 0){
        printf(COLOR_CYAN COLOR_BOLD "─" COLOR_RESET); // top/bottom shield
        } else {
            printf(COLOR_CYAN COLOR_BOLD "|" COLOR_RESET); // left/right shield
        }
    }
    }
}

// Renders slime
void Render::drawEnemy(const Enemy& slime){
    moveTo(slime.x, slime.y+1);
    printf(COLOR_RED COLOR_BOLD "E" COLOR_RESET);
}

// Renders boss
void Render::drawBoss(const class Boss& boss){
    // Determine color based on state
    const char* color = COLOR_RED;  // Normal state
    if (boss.isVulnerable()) {
        color = COLOR_BWHITE;  // Vulnerable state - white
    } else if (boss.isInRageMode()) {
        color = "\033[35m";  // Rage mode - magenta/purple
    }
    
    // Draw 3x3 bos
    // Top row: 0
    int x = boss.x;
    int y = boss.y+1;
    
    moveTo(x, y);
    printf("  ");
    printf("%s0" COLOR_RESET, color);
    
    // Middle row
    moveTo(x - 1, y + 1);
    printf("%s / | \\" COLOR_RESET, color);
    
    // Bottom row
    moveTo(x, y + 2);
    printf("%s / \\" COLOR_RESET, color);
}

// Renders HUD (player HP, boss HP, slime HP, controls)
void Render::drawHUD(const Map& map, int hp, int maxHp, class Boss* boss, bool roomLocked, const std::vector<Slime>& slimes){
    moveTo(0, map.getHeight() + 2);

    if (boss != nullptr) {
        int bosshpClamped = std::max(0, std::min(boss->hp, boss->maxHp));
        int bossHealthPercent = (bosshpClamped * 100) / boss->maxHp;
        
        const char* bossHpColor = COLOR_BGREEN;
        if(bossHealthPercent <= 50) bossHpColor = COLOR_BYELLOW;
        if(bossHealthPercent <= 25) bossHpColor = COLOR_BRED;
        
        printf("%sBOSS HP: [%d/%d] (%d%%)%s\n",
            bossHpColor, boss->hp, boss->maxHp, bossHealthPercent, COLOR_RESET);
    }
    // slime HP list
    if(!slimes.empty()){
        printf("\n");
        for(int i = 0; i < (int)slimes.size(); i++){
            int filled = (slimes[i].hp * 5) / 100;
            const char* barColor = COLOR_BGREEN;
            if(filled <= 2) barColor = COLOR_BYELLOW;
            if(filled <= 1) barColor = COLOR_BRED;
            printf("Slime %d: %s", i + 1, barColor);
            for(int b = 0; b < 3; b++)
                putchar(b < filled ? '#' : '.');
            printf("  " COLOR_RESET);
        }
        printf("\n");
    }
    int hpClamped = std::max(0, std::min(hp, maxHp));
    int hearts = (hpClamped * 3) / maxHp;

    const char* hpColor = COLOR_BGREEN;
    if(hearts == 1) hpColor = COLOR_BYELLOW;
    if(hearts <= 0) hpColor = COLOR_BRED;

    printf("%sHP: %s%s%s  [%d/%d]%s  "
           COLOR_CYAN "[P]" COLOR_RESET "Pause "
           COLOR_CYAN "[Q]" COLOR_RESET "Quit "
           COLOR_CYAN "[SPC]" COLOR_RESET "Atk "
           COLOR_CYAN "[C]" COLOR_RESET "Def",
        hpColor,
        hearts >= 0 ? "<3" : "  ",
        hearts >= 1 ? "<3" : "  ",
        hearts >= 2 ? "<3" : "  ",
        hp, maxHp,
        COLOR_RESET);

    if(roomLocked && boss == nullptr){
        printf("  " COLOR_RED COLOR_BOLD "[LOCKED: %d slime%s left]" COLOR_RESET,
            (int)slimes.size(), slimes.size() == 1 ? "" : "s");
    }

}

void Render::drawAttack(bool isAttacking, int attackX, int attackY, int lastX){
    if(isAttacking){
        moveTo(attackX, attackY+1);
        if(lastX != 0)  printf(COLOR_BYELLOW COLOR_BOLD "─" COLOR_RESET); // right
        else  printf(COLOR_BYELLOW COLOR_BOLD "|" COLOR_RESET); // fallback
    }
}

// Renders map tile based on type
void Render::drawTile(int x, int y, char tile){
    moveTo(x, y);
    if(tile == '|' || tile == '_' || tile == '+'){
        printf("\033[1m%c\033[0m", tile); // bold walls
    } else if(tile == '#'){
        printf(COLOR_YELLOW "%c" COLOR_RESET, tile);
    } else if(tile == 's'){
        printf(" ");
    } else {
        putchar(tile);
    }
}

// Renders locked door overlays
void Render::drawLockedDoors(const Map& map, bool roomLocked){
    if(!roomLocked) return;

    int w = map.getWidth();
    int h = map.getHeight();

    if(map.getNeighbor('N') != "0"){
    for(int x = 1; x < w-1; x++){
        char t = map.getTile(x, 0);
        if(t != '+' && t != '-' && t != '|' && t != 's'){
            moveTo(x, 1);
            printf(COLOR_RED COLOR_BOLD "X" COLOR_RESET);
        }
    }
}
if(map.getNeighbor('S') != "0"){
    for(int x = 1; x < w-1; x++){
        char t = map.getTile(x, h-1);
        if(t != '+' && t != '-' && t != '|' && t != 's'){
            moveTo(x, h);
            printf(COLOR_RED COLOR_BOLD "X" COLOR_RESET);
        }
    }
}
if(map.getNeighbor('E') != "0"){
    for(int y = 1; y < h-1; y++){
        char t = map.getTile(w-1, y);
        if(t != '+' && t != '|' && t != '-' && t != 's'){
            moveTo(w-1, y+1);
            printf(COLOR_RED COLOR_BOLD "X" COLOR_RESET);
        }
    }
}
if(map.getNeighbor('W') != "0"){
    for(int y = 1; y < h-1; y++){
        char t = map.getTile(0, y);
        if(t != '+' && t != '|' && t != '-' && t != 's'){
            moveTo(0, y+1);
            printf(COLOR_RED COLOR_BOLD "X" COLOR_RESET);
        }
    }
}
}