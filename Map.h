#ifndef MAP_H
#define MAP_H

#include <vector>
#include <string>

class Map {
public:
    Map();
    int getWidth() const;
    int getHeight() const;

    bool isWalkable(int x, int y) const;
    char getTile(int x, int y) const;

private:
    std::vector<std::string> grid;
};

#endif