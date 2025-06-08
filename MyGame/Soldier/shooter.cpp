#include <string>

#include "shooter.hpp"

Shooter::Shooter(int x, int y):
Soldier("mapScene/shooter.png", x, y, 10, 50, 100, 10){};

std::string Shooter::soldierName() const{
    return "shooter";
}