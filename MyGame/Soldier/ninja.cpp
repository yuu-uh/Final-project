#include <string>

#include "ninja.hpp"

Ninja::Ninja(int x, int y, int dir):
Soldier("mapScene/ninja.png", x, y, dir, 10, 50, 100, 10){
    attackRadius = 50;
    cooldown = 0.5f;
};

std::string Ninja::soldierName() const{
    return "ninja";
}