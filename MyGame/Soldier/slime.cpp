#include <string>

#include "slime.hpp"

Slime::Slime(int x, int y, int dir):
Soldier("mapScene/slime.png", x, y, dir, 10, 50, 100, 10){
    attackRadius = 50;
    cooldown = 0.5f;
};

std::string Slime::soldierName() const{
    return "slime";
}