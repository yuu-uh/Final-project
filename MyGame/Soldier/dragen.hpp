#ifndef DRAGEN_HPP
#define DRAGEN_HPP
#include <list>
#include <string>
#include <vector>

#include "Engine/Point.hpp"
#include "Engine/Sprite.hpp"
#include "Soldier.hpp"


class Dragen : public Soldier {
public:
    Dragen(int x, int y, int dir, bool isLocal);
    std::string soldierName() const override;
    void CreateWeapon() override;
};
#endif   // ENEMY_HPP
