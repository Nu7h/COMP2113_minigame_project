#ifndef RENDER_H
#define RENDER_H

#include "Map.h"
#include "Player.h"
#include "Enemy.h"
#include <vector>
#include <algorithm>
class Render {
public:
    void drawStartMenu(int menuSelection, bool hasSaveFile);
    void drawDifficultyMenu(int menuSelection);
    void drawGame(const Map& map, const Player& player, const std::vector<Slime>& slimes, class Boss* boss, bool isAttacking, int attackX, int attackY,  bool roomLocked);
    void drawPauseOverlay();
    void drawGameOver();
    void drawWin();
    void drawSavePrompt();

private:
    void drawMap(const Map& map);
    void drawLockedDoors(const Map& map, bool roomLocked);
    void drawPlayer(const Player& player, const Map& map);
    void drawEnemy(const Enemy& enemy);
    void drawBoss(const class Boss& boss);
    void drawProjectile(const slimeProjectile& projectile);
    void drawBossProjectile(const struct bossProjectile& projectile);
    void drawAttack(bool isAttacking, int attackX, int attackY, int lastX);
    void drawHUD(const Map& map, int hp, int maxHp, class Boss* boss);
    void drawTile(int x, int y, char tile);

};

#endif