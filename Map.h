#ifndef MAP_H
#define MAP_H

#include <vector>
#include <string>

struct RoomLinks{
    std::string north, east, south, west;
};

class Map {
public:
    Map();
    bool loadFromFile(const std::string& path);

    const RoomLinks& getLinks(){ return links; }
    std::string getNeighbor(char dir) const; // 'N', 'E', 'S', 'W'

    int getWidth() const;
    int getHeight() const;
    bool isWalkable(int x, int y) const;
    char getTile(int x, int y) const;
    void setTile(int x, int y, char c);

private:
    std::vector<std::string> grid;
    RoomLinks links{"0", "0", "0", "0"};
    std::string roomname;
};

#endif