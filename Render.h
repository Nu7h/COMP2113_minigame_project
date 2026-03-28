#ifndef RENDER_H
#define RENDER_H

#include "Map.h"
#include "Player.h"

class Render {
public:
    void drawMenu(int menuSelection);
    void drawGame(const Map& map, const Player& player, bool isAttacking, int attackX, int attackY);
    void drawPauseOverlay();
    void drawGameOver();
    void drawWin();

private:
    void drawMap(const Map& map);
    void drawPlayer(const Player& player);
    void drawAttack(bool isAttacking, int attackX, int attackY);
    void drawHUD(const Map& map);
    void drawTile(int x, int y, char tile);
};

#endif