#ifndef RENDER_H
#define RENDER_H

#include "Map.h"
#include "Player.h"


class Render {
public:
    void draw(const Map& map, const Player& player);
};

#endif