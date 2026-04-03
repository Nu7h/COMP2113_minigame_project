#include <iostream>
#include <ctime>
#include <vector>
#include <cstdlib>
#include "Enemy.h"

using namespace std;

int min_approaching_tiles = 5;

void Enemy::approach(const Player& player, const Map& map){
    // Save current position
    lastX = x;
    lastY = y;

    // Pick a random direction: 0=Up, 1=Down, 2=Left, 3=Right
    int dir = std::rand() % 4;
    int next_x = x;
    int next_y = y;

    if (dir == 0) {
        next_y -= 1;
    } else if (dir == 1) {
        next_y += 1;
    } else if (dir == 2) {
        next_x -= 1;
    } else if (dir == 3) {
        next_x += 1;
    }

    // Check if the next tile is walkable (not a '#')
    if (map.isWalkable(next_x, next_y)) {
        x = next_x;
        y = next_y;
    }
}
