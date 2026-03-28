#include "Map.h"

Map::Map() {
    grid = {
        "#############",
        "#...........#",
        "#...........#",
        "#....#......#",
        "#...........#",
        "#...........#",
        "#...........#",
        "#############"
    };
}

char Map::getTile(int x, int y) const {
    return grid[y][x];
}

bool Map::isWalkable(int x, int y) const {
    if (y < 0 || y >= (int)grid.size() || x < 0 || x >= (int)grid[0].size()) {
        return false;
    }

    char tile = grid[y][x];

    if (tile == '#') {
        return false;
    }

    return true;
}

int Map::getHeight() const{
    return grid.size();
}

int Map::getWidth() const{
    return grid[0].size();
}