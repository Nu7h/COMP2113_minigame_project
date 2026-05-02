#include "Player.h"
using namespace std;

// Moves player if target tile is walkable and stores last direction
void Player::move(int dx, int dy, const Map& map){
    lastX = dx;
    lastY = dy;
    
    if(map.isWalkable(x + dx, y + dy)){
        x += dx;
        y += dy;
    }
}

// Applies damage with invincibility frames to prevent rapid hits
bool Player::takeDamage(int dmg){
    if(iFrames > 0) return false;  // invincible, ignore hit
    hp -= dmg;
    iFrames = 40;                  // ~2 seconds at 50ms tick
    return true;
}

// Updates invincibility frames and blinking effect
void Player::updateIFrames(){
    if(iFrames > 0) iFrames--;
    isBlinking = (iFrames > 0) && ((iFrames / 4) % 2 == 0);
}

// Activates shield if not already active
void Player::activateShield(){
    if(shieldTimer <= 0){   // can only shield if not already active
        isShielding = true;
        shieldTimer = shieldDuration;
    }
}

// Updates shield duration and deactivates when expired
void Player::updateShield(){
    if(shieldTimer > 0){
        shieldTimer--;
        if(shieldTimer <= 0)
            isShielding = false;
    }
}

// Checks if a projectile is blocked by the shield position
bool Player::blockProjectile(int projX, int projY) const {
    if(!isShielding) return false;
    int sx = x + lastX;
    int sy = y + lastY;
    return projX == sx && projY == sy;
}

// Stores last movement direction for future reference
void Player::setLast(int dx, int dy){
    lastX = dx;
    lastY = dy;
}