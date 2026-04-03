#ifndef PLAYER_H
#define PLAYER_H

#include "Map.h"
#include "Entity.h"

class Player : public Entity {
public:
    Player() : Entity{1, 1} {}
    void move(int dx, int dy, const Map& map, int ex = -1, int ey = -1);

    int getLastX() const { return lastX; }
    int getLastY() const { return lastY; }

private:
    int lastX = 0;
    int lastY = -1;
};

#endif