#include <string>

#include "dragen.hpp"

Dragen::Dragen(int x, int y, int dir):
Soldier("mapScene/dragen.png", x, y, dir ,10, 50, 100, 10){
    attackRadius = 50;
    cooldown = 0.5f;
};

std::string Dragen::soldierName() const{
    return "dragen";
}
