#include <string>

#include "dragen.hpp"

Dragen::Dragen(int x, int y, int dir):
Soldier("mapScene/dragen.png", x, y, dir ,50, 50, 60, 10){
    attackRadius = 100;
    cooldown = 0.5f;
};

std::string Dragen::soldierName() const{
    return "dragen";
}
