#ifndef ENTITY_H
#define ENTITY_H

struct Entity {
    int x, y;

    int getX() const { return x; }
    int getY() const { return y; }
};

#endif