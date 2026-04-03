#ifndef INPUT_H
#define INPUT_H

class Input {
public:
    void update();
    int getKey() const;

private:
    int key;
};

#endif