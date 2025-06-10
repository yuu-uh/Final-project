#ifndef Arrow_HPP
#define Arrow_HPP
#include "Bullet.hpp"

class Enemy;
class Turret;
namespace Engine {
    struct Point;
}   // namespace Engine

class Arrow : public Bullet {
public:
    explicit Arrow(Engine::Point position, Engine::Point forwardDirection, float rotation, Soldier *parent);
    void OnExplode(Soldier *enemy) override;
};
#endif   // Arrow_HPP
