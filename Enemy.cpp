#include <cstdlib>
#include "Enemy.h"

using namespace std;

int max_approaching_tiles = 10;
const int slimeShootCooldownTicks = 20;

void Enemy::approach(const Player& player, const Map& map, const std::vector<Slime>& others){
    if (shootCooldown > 0) {
        shootCooldown--;
    }

    int px = player.getX(); 
    int py = player.getY();
    int distance = std::abs(px-x) + std::abs(py-y);

    if (distance <= 4){
        if (shootCooldown <= 0){
            particles.push_back({x, y, (px > x ? 1 : (px < x ? -1 : 0)), (py > y ? 1 : (py < y ? -1 : 0)), 10});
            shootCooldown = slimeShootCooldownTicks;
        }
        return;
    }

    if(distance > max_approaching_tiles || distance == 4) return;

    if(moveCooldown > 0){
        moveCooldown--;
        return;
    }

    moveCooldown = maxmoveCooldown;
    
    int adx = px - x; 
    int ady = py - y; // which quatile the player is to the Enemy
    int dx = 0, dy = 0;

    if(std::abs(adx) > std::abs(ady)){
        if(adx != 0)
            dx = (adx > 0) ? 1 : -1;
    } else {
        if(ady != 0)
            dy = (ady > 0) ? 1 : -1;
    }

    int nx = x + dx;
    int ny = y + dy;

    bool occupied = false;
    for (const auto& s : others) {
        if (&s != static_cast<const Slime*>(this) && s.x == nx && s.y == ny) {
            occupied = true;
            break;
        }
    }

    if (occupied || !map.isWalkable(nx, ny)){
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
            it = particles.erase(it);
        } else if (!map.isWalkable(it->x, it->y)){
            it = particles.erase(it);
        } else {
            ++it;
        }
    }
}
