#ifndef SLIME_HPP
#define SLIME_HPP
#include <list>
#include <string>
#include <vector>

#include "Engine/Point.hpp"
#include "Engine/Sprite.hpp"
#include "Soldier.hpp"


class Slime : public Soldier {
public:
    Slime(int x, int y, int dir, bool isLocal);
    std::string soldierName() const override;
    void CreateWeapon() override;

};
#endif   // ENEMY_HPP
