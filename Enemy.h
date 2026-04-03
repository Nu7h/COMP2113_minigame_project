#ifndef ENEMY_H
#define ENEMY_H

#include "Map.h"
#include "Entity.h"
#include "Player.h"

class Player;

class Enemy : public Entity {
    public:
        Enemy() : Entity{1, 6} {}
        virtual ~Enemy() = default;
        virtual void update(const Map& map);
        void approach(const Player& player, const Map& map);
        int getLastX() const { return lastX; }
        int getLastY() const { return lastY; }

    protected:
        int dx = 1;
        int dy = 0;
        int moveTimer = 0;
    private:
        int lastX = 0;
        int lastY = -1;
};

class Slime : public Enemy{

    private:
        int hp = 100;
        int min_atk = 0;
        int max_atk = 10;
};

class Boss : public Enemy{
    public:
        void update(const Map& map) override {}
    private:
        int hp = 1000;
        int min_atk = 10;
        int max_atk = 40;
        int healing = 10;
};
#endif
