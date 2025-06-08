#ifndef MASTER_HPP
#define MASTER_HPP
#include <list>
#include <string>
#include <vector>

#include "Engine/Point.hpp"
#include "Engine/Sprite.hpp"
#include "Soldier.hpp"


class Master : public Soldier {
public:
    Master(int x, int y);
    std::string soldierName() const override;
};
#endif   // ENEMY_HPP
