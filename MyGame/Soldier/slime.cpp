#include <string>

#include "slime.hpp"

Slime::Slime(int x, int y, int dir, bool isLocal):
Soldier("mapScene/slime.png", x, y, dir, 10, 50, 100, 10, true){
    attackRadius = 100;
    cooldown = 0.5f;
};

std::string Slime::soldierName() const{
    return "slime";
}