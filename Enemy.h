#ifndef ENEMY_H
#define ENEMY_H

#include "Map.h"
#include "Entity.h"
#include "Player.h"

class Player;
class Slime;

struct slimeProjectile{
    int x, y;
    int dx, dy;
    int moveCooldown = 10;
};

class Enemy : public Entity {
    public:
        Enemy() : Entity{1, 6} {}
        void approach(const Player& player, const Map& map,const std::vector<Slime>& others);
        void setmaxmoveCooldown(int maxCooldown) {maxmoveCooldown = maxCooldown;}
        void updateProjectiles(const Map& map, Player& player);
        int getLastX() const { return lastX; }
        int getLastY() const { return lastY; }
        std::vector<slimeProjectile> particles;

    private:
        int lastX = 0;
        int lastY = -1;
        int moveCooldown = 0;
        int maxmoveCooldown = 8;
        int shootCooldown = 0;
        int wanderCooldown = 0;
        int wanderDx = 0;
        int wanderDy = 0;
};


class Slime : public Enemy{

    public:
        int hp = 100;

    private:
        int min_atk = 0;
        int max_atk = 10;
};

class Boss : public Enemy{
    private:
        int hp = 1000;
        int min_atk = 10;
        int max_atk = 40;
        int healing = 10;
};
#endif
