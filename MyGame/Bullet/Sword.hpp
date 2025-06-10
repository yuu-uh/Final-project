// Sword.hpp
#ifndef SWORD_HPP
#define SWORD_HPP

#include "Bullet/Bullet.hpp"

class Sword : public Bullet {
public:
    float lifeTime;           
    float elapsed;            

    Sword(Engine::Point position, Engine::Point forwardDirection, float rotation, Soldier* parent);

    void Update(float deltaTime) override;
    //void OnExplode(Soldier* enemy) override;
};

#endif // SWORD_HPP
