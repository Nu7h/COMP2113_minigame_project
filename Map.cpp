#include "Map.h"
#include <sstream>
#include <fstream>
#include <iostream>

Map::Map() = default;

bool Map::loadFromFile(const std::string& path) {
    std::ifstream in(path);
    if(!in) return false;

    std::string line;
    if (!std::getline(in, line)) return false;

    std::stringstream ss(line);
    if (!(ss >> links.north >> links.east >> links.south >> links.west)) return false;

    if (!std::getline(in, roomname)) return false;

    grid.clear();
    while(std::getline(in, line)){
        if(line.empty()) continue;
        grid.push_back(line);
    }

    if(grid.empty()) return false;
    return true;
}

std::string Map::getNeighbor(char dir) const {
    switch (dir) {
        case 'N': return links.north;
        case 'E': return links.east;
        case 'S': return links.south;
        case 'W': return links.west;
        default:  return "0";
    }
}

char Map::getTile(int x, int y) const {
    return grid[y][x];
}

void Map::setTile(int x, int y, char c) {
    if (y >= 0 && y < (int)grid.size() && x >= 0 && x < (int)grid[y].size())
        grid[y][x] = c;
}

bool Map::isWalkable(int x, int y) const {
    if (y < 0 || y >= (int)grid.size() || x < 0 || x >= (int)grid[0].size())
        return false;

    char tile = grid[y][x];
    return tile != '+' && tile != '-' && tile != '|' && tile != '#' && tile != 's';
}

int Map::getHeight() const{
    return grid.size();
}

int Map::getWidth() const{
    return grid[0].size();
}

std::string Map::getRoomName() const {
    return roomname;
}