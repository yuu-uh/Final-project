#ifndef NINJA_HPP
#define NINJA_HPP
#include <list>
#include <string>
#include <vector>

#include "Engine/Point.hpp"
#include "Engine/Sprite.hpp"
#include "Soldier.hpp"


class Ninja : public Soldier {
public:
    Ninja(int x, int y);
    std::string soldierName() const override;
};
#endif   // ENEMY_HPP
