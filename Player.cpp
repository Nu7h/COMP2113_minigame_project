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

bool Player::takeDamage(int dmg){
    if(iFrames > 0) return false;  // invincible, ignore hit
    hp -= dmg;
    iFrames = 40;                  // ~2 seconds at 50ms tick
    return true;
}

void Player::updateIFrames(){
    if(iFrames > 0) iFrames--;
    isBlinking = (iFrames > 0) && ((iFrames / 4) % 2 == 0);
}