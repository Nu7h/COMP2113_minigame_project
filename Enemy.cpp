#include <iostream>
#include <ctime>
#include <vector>
#include "Map.h"
#include <cstdlib>
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
