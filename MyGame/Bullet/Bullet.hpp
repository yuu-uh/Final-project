#ifndef BULLET_HPP
#define BULLET_HPP
#include <string>

#include "Engine/Sprite.hpp"
#include "Soldier/Soldier.hpp"

class Soldier;
class PlayScene;
namespace Engine {
    struct Point;
}   // namespace Engine

class Bullet : public Engine::Sprite {
protected:
    float speed;
    float damage;
    Soldier *parent;
    PlayScene *getPlayScene();
    virtual void OnExplode(Soldier *enemy);

public:
    Soldier *Target = nullptr;
    explicit Bullet(std::string img, float speed, float damage, Engine::Point position, Engine::Point forwardDirection, float rotation, Soldier *parent);
    void Update(float deltaTime) override;
};
#endif   // BULLET_HPP
