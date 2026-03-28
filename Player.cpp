#include "Player.h"
#include "Map.h"
using namespace std;

void Player::move(int dx, int dy, const Map& map){
    if(map.isWalkable(x + dx, y + dy)){
        x += dx;
        y += dy;

        lastX = dx;
        lastY = dy;
    }
}