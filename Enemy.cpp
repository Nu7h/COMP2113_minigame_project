#include <iostream>
#include <ctime>
#include <cstdlib>
#include <vector>
#include "Enemy.h"

using namespace std;

int min_approaching_tiles = 5;

void Enemy::update(const Map& map, int px, int py) {
    moveTimer++;
    // Game loop runs at ~20 frames per second (50ms per frame)
    // 14 frames = 0.7 seconds
    if (moveTimer >= 14) {
        moveTimer = 0;
        
        int dirs[4][2] = {{0,1}, {0,-1}, {1,0}, {-1,0}};
        int startIdx = rand() % 4;
        
        int fallbackDx = 0;
        int fallbackDy = 0;
        bool foundValid = false;
        
        for (int i = 0; i < 4; ++i) {
            int idx = (startIdx + i) % 4;
            int cx = dirs[idx][0];
            int cy = dirs[idx][1];
            int nx = x + cx;
            int ny = y + cy;
            
            if (map.isWalkable(nx, ny) && (nx != px || ny != py)) {
                if (!foundValid) {
                    fallbackDx = cx;
                    fallbackDy = cy;
                    foundValid = true;
                }
                
                // If this direction is NOT going directly backward
                if (!(cx == -dx && cy == -dy)) {
                    dx = cx;
                    dy = cy;
                    lastX = x;
                    lastY = y;
                    x = nx;
                    y = ny;
                    return; // successfully moved
                }
            }
        }
        
        // If we reach here, the ONLY valid move was going backward, or there were no valid moves
        if (foundValid) {
            dx = fallbackDx;
            dy = fallbackDy;
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
