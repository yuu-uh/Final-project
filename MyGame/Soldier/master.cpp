#include <string>

#include "master.hpp"

Master::Master(int x, int y, int dir):
Soldier("mapScene/master.png", x, y, dir, 10, 50, 100, 10){
    attackRadius = 100;
    cooldown = 0.5f;
};

std::string Master::soldierName() const{
    return "master";
}
