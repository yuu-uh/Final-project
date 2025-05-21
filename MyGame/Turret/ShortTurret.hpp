#ifndef ShortTurret_HPP
#define ShortTurret_HPP
#include "Turret.hpp"

class ShortTurret : public Turret {
public:
    static const int Price;
    ShortTurret(float x, float y);
    void CreateBullet() override;
};
#endif   
