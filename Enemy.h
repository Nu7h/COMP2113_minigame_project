#ifndef ENEMY_H
#define ENEMY_H

#include "Map.h"
#include "Entity.h"
#include "Player.h"
#include "Difficulty.h"
#include <iostream>
#include <ctime>
#include <vector>
#include <cstdlib>

class Player;
class Slime;
class Boss;


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
        int hp = 60;

};

struct bossProjectile {
    int x, y;
    int dx, dy;
    int moveCooldown = 10;
    bool tracked = false;        // re-aims toward (targetX, targetY) each step
    int targetX = 0;
    int targetY = 0;
    bool splitOnArrival = false; // on reaching target, spawn 8-dir spray and expire
};

class Boss : public Enemy{

    public:
        Boss() : Enemy() {}
        int hp = 1000;
        int maxHp = 1000;
        Difficulty difficulty = Difficulty::EASY;
        std::vector<bossProjectile> bossProjectiles;
        
        // Boss behavior methods
        void updateBoss(const Player& player, Map& map);
        void updateBossProjectiles(const Map& map, Player& player);
        
        // Getters
        int getAttackPhase() const { return attackPhase; }
        bool isInRageMode() const { return hp <= 250; }  // 25% of 1000
        bool isVulnerable() const { return vulnerableTimer > 0; }
        int getVulnerableTimer() const { return vulnerableTimer; }
        int getRageJumpCount() const { return rageJumpCount; }
        

        bool isHitByAttack(int attackX, int attackY) const {
            if (attackX == x + 2 && attackY == y)     return true;  // Head
            if (attackX == x     && attackY == y + 1) return true;  // Left arm
            if (attackX == x + 2 && attackY == y + 1) return true;  // Torso
            if (attackX == x + 4 && attackY == y + 1) return true;  // Right arm
            if (attackX == x + 1 && attackY == y + 2) return true;  // Left leg
            if (attackX == x + 3 && attackY == y + 2) return true;  // Right leg
            return false;
        }
        
    private:
        // Attack pattern tracking
        int attackCooldown = 0;
        int shootCooldown = 0;
        int jumpCooldown = 0;
        int attackPhase = 0;  // counts attacks to determine pattern
        int lastPlayerX = 0;
        int lastPlayerY = 0;
        
        // Rage mode tracking
        int rageJumpCount = 0;    // count jumps in rage mode
        int vulnerableTimer = 0;   // countdown for vulnerable state
        
        // Constants for timing (assuming 50ms per tick)
        static constexpr int NORMAL_SHOOT_TICKS = 100;  // 5 seconds
        static constexpr int RAGE_SHOOT_TICKS = 70;     // 3.5 seconds
        static constexpr int JUMP_TICKS = 40;           // ~2 seconds for jump
        static constexpr int VULNERABLE_DURATION = 60;  // 3 seconds
        static constexpr int APPROACH_TICKS = 8;        // ~0.4 seconds between steps

        int approachCooldown = 0;
};
#endif
