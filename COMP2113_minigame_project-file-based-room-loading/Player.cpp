#include "Player.h"
#include "Map.h"
using namespace std;

void Player::move(int dx, int dy, const Map& map){
    lastX = dx;
    lastY = dy;
    
    if(map.isWalkable(x + dx, y + dy)){
        x += dx;
        y += dy;
    }
}