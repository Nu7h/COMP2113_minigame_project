#ifndef RENDER_H
#define RENDER_H

#include "Map.h"
#include "Player.h"
#include "Enemy.h"
#include <vector>

class Render {
public:
    void drawMenu(int menuSelection);
    void drawGame(const Map& map, const Player& player, const std::vector<Slime>& slimes, bool isAttacking, int attackX, int attackY);
    void drawPauseOverlay();
    void drawGameOver();
    void drawWin();
    void drawSavePrompt();
    void drawContinuePrompt();

private:
    void drawMap(const Map& map);
    void drawPlayer(const Player& player);
    void drawEnemy(const Enemy& enemy);
    void drawProjectile(const slimeProjectile& projectile);
    void drawAttack(bool isAttacking, int attackX, int attackY);
    void drawHUD(const Map& map);
    void drawTile(int x, int y, char tile);

};

#endif