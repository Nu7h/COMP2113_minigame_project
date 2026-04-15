#ifndef PLAYER_H
#define PLAYER_H

#include "Map.h"
#include "Entity.h"

class Player : public Entity {
public:
    Player() : Entity{1, 1} {}
    void move(int dx, int dy, const Map& map);
    void updateIFrames();

    int getLastX() const { return lastX; }
    int getLastY() const { return lastY; }
    int hp = 100;
    int maxHp = 100;
    int iFrames = 0;          // ticks of invincibility remaining
    bool isBlinking = false; 
    bool takeDamage(int dmg);

private:
    int lastX = 0;
    int lastY = 0;
};

#endif