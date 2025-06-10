#ifndef Knife_HPP
#define Knife_HPP
#include "Bullet.hpp"

class Enemy;
class Turret;
namespace Engine {
    struct Point;
}   // namespace Engine

class Knife : public Bullet {
public:
    explicit Knife(Engine::Point position, Engine::Point forwardDirection, float rotation, Soldier *parent);
    void OnExplode(Soldier *enemy) override;
};
#endif   // Knife_HPP
