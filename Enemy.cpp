#include <cstdlib>
#include "Enemy.h"

using namespace std;

int max_approaching_tiles = 10;

void Enemy::approach(const Player& player, const Map& map){
    int px = player.getX(); 
    int py = player.getY();
    int distance = std::abs(px-x) + std::abs(py-y);

    if(distance <= max_approaching_tiles && distance != 0){
        int adx = px - x; 
        int ady = py - y; // which quatile the player is to the Enemy
        int dx, dy = 0; 
        if(std::abs(adx) > std::abs(ady)){
            if(adx != 0)
                dx = (adx > 0) ? 1 : -1;
        } else {
            if(ady != 0)
                dy = (ady > 0) ? 1 : -1;
        }
        int tempx = x + dx;
        int tempy = y + dy;
        if(map.isWalkable(tempx,tempy)){
            x += dx;
            y += dy;
        }
    }
}
