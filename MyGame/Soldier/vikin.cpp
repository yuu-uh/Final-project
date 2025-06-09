#include <string>

#include "vikin.hpp"

Vikin::Vikin(int x, int y, int dir, bool isLocal):
Soldier("mapScene/vikin.png", x, y, dir, 10, 50, 100, 10, true){
    attackRadius = 100;
    cooldown = 0.5f;
};

std::string Vikin::soldierName() const{
    return "vikin";
}
