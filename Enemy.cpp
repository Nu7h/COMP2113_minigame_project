#include <iostream>
#include <ctime>
#include <vector>
#include "Map.h"
#include <cstdlib>
#include <algorithm>
#include "Enemy.h"

using namespace std;

int max_approaching_tiles = 10;
const int slimeShootCooldownTicks = 20;

void Enemy::approach(const Player& player, const Map& map, const std::vector<Slime>& others){
    if (shootCooldown > 0) shootCooldown--;

    int px = player.getX(); 
    int py = player.getY();
    int distance = std::abs(px-x) + std::abs(py-y);

    // shoot when close
    if (distance <= 4){
        if (shootCooldown <= 0){
            particles.push_back({x, y, (px > x ? 1 : (px < x ? -1 : 0)), (py > y ? 1 : (py < y ? -1 : 0)), 10});
            shootCooldown = slimeShootCooldownTicks;
        }
        return;
    }

    if(moveCooldown > 0){
        moveCooldown--;
        return;
    }

    int dx = 0, dy = 0;

    if (distance <= max_approaching_tiles){
        moveCooldown = maxmoveCooldown;
        // chase player
        int adx = px - x; 
        int ady = py - y;
        if(std::abs(adx) > std::abs(ady)){
            dx = (adx > 0) ? 1 : -1;
        } else {
            dy = (ady > 0) ? 1 : -1;
        }
    } else {
        moveCooldown = maxmoveCooldown*2;
        // random wander when player is far
            // pick a new random direction every ~10 moves
            wanderCooldown = 8 + std::rand() % 8;
            int dir = std::rand() % 5; // 5th option = stay still briefly
            if      (dir == 0) { dx =  1; dy =  0; }
            else if (dir == 1) { dx = -1; dy =  0; }
            else if (dir == 2) { dx =  0; dy =  1; }
            else if (dir == 3) { dx =  0; dy = -1; }
            // dir == 4: dx=dy=0, pause
            wanderDx = dx;
            wanderDy = dy;
    }

    if (dx == 0 && dy == 0) return;

    int nx = x + dx;
    int ny = y + dy;

    // check collision with other slimes
    bool occupied = false;
    for (const auto& s : others) {
        if (&s != static_cast<const Slime*>(this) && s.x == nx && s.y == ny) {
            occupied = true;
            break;
        }
    }

    if (occupied || !map.isWalkable(nx, ny)){
        // hit a wall while wandering, pick new direction next tick
        wanderCooldown = 0;
        return;
    }

    x = nx;
    y = ny;
}

void Enemy::updateProjectiles(const Map& map, Player& player){
    for ( auto it = particles.begin(); it != particles.end();){
        if (it->moveCooldown > 0) {
            it->moveCooldown--;
            ++it;
            continue;
        }

        it->x += it->dx;
        it->y += it->dy;
        it->moveCooldown = 4;

        if(it->x == player.getX() && it->y == player.getY()){
            player.takeDamage(10);
            it = particles.erase(it);
        } else if(player.blockProjectile(it->x, it->y)){
            it = particles.erase(it);
        } else if (!map.isWalkable(it->x, it->y)){
            it = particles.erase(it);
        } else {
            ++it;
        }
    }
}

// ====== BOSS IMPLEMENTATION ======

void Boss::updateBoss(const Player& player, Map& map) {
    lastPlayerX = player.getX();
    lastPlayerY = player.getY();

    // Handle vulnerable state
    if (vulnerableTimer > 0) {
        vulnerableTimer--;
        return;  // Boss doesn't move or shoot while vulnerable
    }

    // Determine shoot cooldown based on mode
    int shootCooldownTicks = isInRageMode() ? RAGE_SHOOT_TICKS : NORMAL_SHOOT_TICKS;

    if (shootCooldown > 0) {
        shootCooldown--;
    } else {
        // EASY: instant 8-direction spray from the boss.
        // NORMAL/HARD: fire ONE shot that homes to the player's last position;
        // when it arrives there it splits into the 8-direction spray.
        bool aimedAttack = (difficulty == Difficulty::NORMAL || difficulty == Difficulty::HARD);
        int aimDx = (lastPlayerX > x) - (lastPlayerX < x);
        int aimDy = (lastPlayerY > y) - (lastPlayerY < y);

        if (aimedAttack && (aimDx != 0 || aimDy != 0)) {
            bossProjectile p;
            p.x = x;
            p.y = y;
            p.dx = aimDx;
            p.dy = aimDy;
            p.moveCooldown = 10;
            p.tracked = true;
            p.targetX = lastPlayerX;
            p.targetY = lastPlayerY;
            p.splitOnArrival = true;
            bossProjectiles.push_back(p);
        } else {
            // Easy mode (or boss is on the player's tile): spray immediately.
            for (int dir = 0; dir < 8; ++dir) {
                int dx = 0, dy = 0;
                if      (dir == 0) { dx = 0;  dy = -1; }   // N
                else if (dir == 1) { dx = 1;  dy = -1; }   // NE
                else if (dir == 2) { dx = 1;  dy = 0;  }   // E
                else if (dir == 3) { dx = 1;  dy = 1;  }   // SE
                else if (dir == 4) { dx = 0;  dy = 1;  }   // S
                else if (dir == 5) { dx = -1; dy = 1;  }   // SW
                else if (dir == 6) { dx = -1; dy = 0;  }   // W
                else if (dir == 7) { dx = -1; dy = -1; }   // NW
                bossProjectiles.push_back({x, y, dx, dy, 10});
            }
        }

        shootCooldown = shootCooldownTicks;
        attackPhase++;

        // Every 6th attack, jump to player's last position
        if (attackPhase % 6 == 0) {
            jumpCooldown = JUMP_TICKS;
        }
    }

    // Handle jump — breaks '#' obstacles on the path
    if (jumpCooldown > 0) {
        jumpCooldown--;

        if (jumpCooldown % 2 == 0) {  // Move every other tick
            int dx = 0, dy = 0;

            if (x < lastPlayerX) dx = 1;
            else if (x > lastPlayerX) dx = -1;

            if (y < lastPlayerY) dy = 1;
            else if (y > lastPlayerY) dy = -1;

            int nx = x + dx;
            int ny = y + dy;

            if (map.isWalkable(nx, ny)) {
                x = nx;
                y = ny;
            } else if (map.getTile(nx, ny) == '#') {
                map.setTile(nx, ny, ' ');
                x = nx;
                y = ny;
            }
        }

        // After jump completes, check for rage mode jump counter
        if (jumpCooldown == 0 && isInRageMode()) {
            rageJumpCount++;

            // Every 2 jumps in rage mode, enter vulnerable state
            if (rageJumpCount % 2 == 0) {
                vulnerableTimer = VULNERABLE_DURATION;
                rageJumpCount = 0;
            }
        }
    } else {
        /*
        if (approachCooldown > 0) {
            approachCooldown--;
        } else {
            approachCooldown = APPROACH_TICKS;
            int adx = lastPlayerX - x;
            int ady = lastPlayerY - y;
            int dx = 0, dy = 0;
            if (std::abs(adx) > std::abs(ady)) dx = (adx > 0) ? 1 : -1;
            else if (ady != 0)                  dy = (ady > 0) ? 1 : -1;

            int nx = x + dx;
            int ny = y + dy;
            if (map.isWalkable(nx, ny)) {
                x = nx;
                y = ny;
            }
        }*/
    }
}

void Boss::updateBossProjectiles(const Map& map, Player& player) {
    // Collect newly spawned projectiles separately so we don't invalidate
    // the iterator (push_back can grow the vector mid-iteration).
    std::vector<bossProjectile> spawned;

    for (auto it = bossProjectiles.begin(); it != bossProjectiles.end(); ) {
        // Tracked shot reached its snapshot target without hitting player.
        if (it->tracked && it->x == it->targetX && it->y == it->targetY) {
            if (it->splitOnArrival) {
                int sx = it->x, sy = it->y;
                for (int dir = 0; dir < 8; ++dir) {
                    int dx = 0, dy = 0;
                    if      (dir == 0) { dx = 0;  dy = -1; }
                    else if (dir == 1) { dx = 1;  dy = -1; }
                    else if (dir == 2) { dx = 1;  dy = 0;  }
                    else if (dir == 3) { dx = 1;  dy = 1;  }
                    else if (dir == 4) { dx = 0;  dy = 1;  }
                    else if (dir == 5) { dx = -1; dy = 1;  }
                    else if (dir == 6) { dx = -1; dy = 0;  }
                    else if (dir == 7) { dx = -1; dy = -1; }
                    bossProjectile sp;
                    sp.x = sx;
                    sp.y = sy;
                    sp.dx = dx;
                    sp.dy = dy;
                    sp.moveCooldown = 0;  // start moving immediately
                    spawned.push_back(sp);
                }
            }
            it = bossProjectiles.erase(it);
            continue;
        }

        if (it->moveCooldown > 0) {
            it->moveCooldown--;
            ++it;
            continue;
        }

        // Tracked shots re-aim each step toward the snapshot target
        if (it->tracked) {
            it->dx = (it->targetX > it->x) - (it->targetX < it->x);
            it->dy = (it->targetY > it->y) - (it->targetY < it->y);
        }

        it->x += it->dx;
        it->y += it->dy;
        it->moveCooldown = 4;

        if (it->x == player.getX() && it->y == player.getY()) {
            player.takeDamage(15);  // Boss projectiles do more damage
            // HARD: boss heals 100 HP on a successful hit
            if (difficulty == Difficulty::HARD) {
                hp = std::min(hp + 100, maxHp);
            }
            it = bossProjectiles.erase(it);
        } else if (player.blockProjectile(it->x, it->y)) {
            it = bossProjectiles.erase(it);
        } else if (!map.isWalkable(it->x, it->y)) {
            it = bossProjectiles.erase(it);
        } else {
            ++it;
        }
    }

    for (auto& sp : spawned) {
        bossProjectiles.push_back(sp);
    }
}
