#include <string>

#include "shooter.hpp"

Shooter::Shooter(int x, int y, int dir, bool isLocal):
Soldier("mapScene/shooter.png", x, y, dir, 10, 50, 100, 10, true){
    attackRadius = 100;
    cooldown = 0.5f;
};

std::string Shooter::soldierName() const{
    return "shooter";
}