#include "Render.h"

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

void Render::drawMenu(int menuSelection){
    clearScreen();
    moveTo(4, 3); printf(COLOR_BYELLOW COLOR_BOLD "  THE LEGEND OF HKU" COLOR_RESET);
    moveTo(4, 5); printf(COLOR_BWHITE "  Select difficulty" COLOR_RESET);
    moveTo(4, 7); printf("%s " COLOR_BGREEN "EASY" COLOR_RESET,     menuSelection == 0 ? ">" : " ");
    moveTo(4, 8); printf("%s " COLOR_BYELLOW "NORMAL" COLOR_RESET,  menuSelection == 1 ? ">" : " ");
    moveTo(4, 9); printf("%s " COLOR_BRED "HARD" COLOR_RESET,       menuSelection == 2 ? ">" : " ");
    moveTo(4, 10); printf("%s Quit",                                 menuSelection == 3 ? ">" : " ");
    moveTo(4, 12); printf(COLOR_CYAN "W/S" COLOR_RESET " to move, " COLOR_CYAN "Enter" COLOR_RESET " to select \n\n");
    fflush(stdout);
}

void Render::drawGame(const Map& map, const Player& player, const std::vector<Slime>& slimes, class Boss* boss, bool isAttacking, int attackX, int attackY){
    clearScreen();
    drawMap(map);
    
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
    drawPlayer(player);
    drawHUD(map, player.hp, player.maxHp, boss);
    fflush(stdout);
}

void Render::drawProjectile(const slimeProjectile& projectile){
    moveTo(projectile.x, projectile.y);
    printf(COLOR_BRED COLOR_BOLD "o" COLOR_RESET);
}

void Render::drawBossProjectile(const struct bossProjectile& projectile){
    moveTo(projectile.x, projectile.y);
    printf(COLOR_BRED COLOR_BOLD "*" COLOR_RESET);
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

void Render::drawContinuePrompt(){
    clearScreen();
    moveTo(4, 3); printf(COLOR_BYELLOW COLOR_BOLD "Save file found!" COLOR_RESET);
    moveTo(4, 5); printf("Continue from where you left off?");
    moveTo(4, 7); printf(COLOR_BGREEN "[Y] Yes" COLOR_RESET "   " COLOR_BRED "[N] No (start fresh)" COLOR_RESET);
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
    if(player.isBlinking) return;

    moveTo(player.x, player.y);
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
        int sy = player.y + player.getLastY();
        moveTo(sx, sy);
        if(ly != 0){
        printf(COLOR_CYAN COLOR_BOLD "─" COLOR_RESET); // top/bottom shield
        } else {
            printf(COLOR_CYAN COLOR_BOLD "|" COLOR_RESET); // left/right shield
        }
    }
}

void Render::drawEnemy(const Enemy& slime){
    moveTo(slime.x, slime.y);
    printf(COLOR_RED COLOR_BOLD "E" COLOR_RESET);
}

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
    int y = boss.y;
    
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

void Render::drawHUD(const Map& map, int hp, int maxHp, class Boss* boss){
    moveTo(0, map.getHeight() + 1);
    

    if (boss != nullptr) {
        int bosshpClamped = std::max(0, std::min(boss->hp, boss->maxHp));
        int bossHealthPercent = (bosshpClamped * 100) / boss->maxHp;
        
        const char* bossHpColor = COLOR_BGREEN;
        if(bossHealthPercent <= 50) bossHpColor = COLOR_BYELLOW;
        if(bossHealthPercent <= 25) bossHpColor = COLOR_BRED;
        
        printf("%sBOSS HP: %s[%d/%d] (%d%%)%s  "
            ,
            bossHpColor,
            bossHpColor,
            boss->hp, boss->maxHp,
            bossHealthPercent,
            COLOR_RESET);
        printf("\n");
    }
        // Regular player HP display
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
}

void Render::drawAttack(bool isAttacking, int attackX, int attackY, int lastX){
    if(isAttacking){
        moveTo(attackX, attackY);
        if(lastX != 0)  printf(COLOR_BYELLOW COLOR_BOLD "─" COLOR_RESET); // right
        else  printf(COLOR_BYELLOW COLOR_BOLD "|" COLOR_RESET); // fallback
    }
}

void Render::drawTile(int x, int y, char tile){
    moveTo(x, y);
    if(tile == '|' || tile == '_' || tile == '+'){
        printf("\033[1m%c\033[0m", tile); // bold walls
    } else if(tile == '#'){
        printf(COLOR_YELLOW "%c" COLOR_RESET, tile);
    } else {
        putchar(tile);
    }
}