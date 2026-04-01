#include <iostream>
#include <ctime>
#include <vector>
#include "Map.h"

using namespace std;

class baseEnemy
{
    public:
    void idle(){

    }
    void moving(){

    }
    void takingDMG(){
        
    }
    int attacking(){

    }
    
};

class  Slime : public baseEnemy
{
    private:
    int hp = 100;
    int min_atk = 0;
    int max_atk = 10;
};

class Boss : public baseEnemy
{
    private:
    int hp = 1000;
    int min_atk = 10;
    int max_atk = 40;
    int healing = 10;
};
