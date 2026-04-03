#include "Player.h"
#include "Map.h"
using namespace std;

void Player::move(int dx, int dy, const Map& map, int ex, int ey){
    lastX = dx;
    lastY = dy;
    
    int nx = x + dx;
    int ny = y + dy;
    if(map.isWalkable(nx, ny)){
        if(nx == ex && ny == ey) return;
        x = nx;
        y = ny;
    }
}