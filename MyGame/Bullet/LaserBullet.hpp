#ifndef LASERBULLET_HPP
#define LASERBULLET_HPP
#include "Bullet.hpp"

class Soldier;
class Turret;
namespace Engine {
    struct Point;
}   // namespace Engine

class LaserBullet : public Bullet {
public:
    explicit LaserBullet(Engine::Point position, Engine::Point forwardDirection, float rotation);
    void OnExplode(Soldier *Soldier) override;
};
#endif   // LASERBULLET_HPP
