#include <iostream>
#include <ctime>
#include <cstdlib>
#include <vector>
#include "Enemy.h"

using namespace std;

int min_approaching_tiles = 5;

void Enemy::update(const Map& map) {
    moveTimer++;
    // Game loop runs at ~30 frames per second
    if (moveTimer >= 30) {
        moveTimer = 0;
        
        // If the current direction is blocked, pick a new random direction
        if (!map.isWalkable(x + dx, y + dy)) {
            int dirs[4][2] = {{0,1}, {0,-1}, {1,0}, {-1,0}};
            int startIdx = rand() % 4;
            for (int i = 0; i < 4; ++i) {
                int idx = (startIdx + i) % 4;
                if (map.isWalkable(x + dirs[idx][0], y + dirs[idx][1])) {
                    dx = dirs[idx][0];
                    dy = dirs[idx][1];
                    break;
                }
            }
        }
        
        // Move if possible
        if (map.isWalkable(x + dx, y + dy)) {
            lastX = x;
            lastY = y;
            x += dx;
            y += dy;
        }
    }
}

void Enemy::approach(const Player& player, const Map& map){
    int px = player.getLastX();
    int py = player.getLastY();


}
