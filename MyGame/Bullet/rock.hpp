#ifndef FIREBULLET_HPP
#define FIREBULLET_HPP
#include "Bullet.hpp"

class Enemy;
class Turret;
namespace Engine {
    struct Point;
}   // namespace Engine

class Rock : public Bullet {
public:
    explicit Rock(Engine::Point position, Engine::Point forwardDirection, float rotation, Soldier *parent);
    void OnExplode(Soldier *enemy) override;
};
#endif   // FIREBULLET_HPP
