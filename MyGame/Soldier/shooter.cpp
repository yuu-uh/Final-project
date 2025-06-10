#include <string>

#include "shooter.hpp"
#include "Scene/PlayScene.hpp"
#include "Bullet/Arrow.hpp"

Shooter::Shooter(int x, int y, int dir, bool isLocal):
Soldier("mapScene/shooter.png", x, y, dir, 10, 50, 100, 10, true){
    attackRadius = 200;
    cooldown = 0.5f;
};

std::string Shooter::soldierName() const{
    return "shooter";
}


void Shooter::CreateWeapon() {
    Engine::Point diff;
    float rotation;

    if (target) {
        diff = target->Position - Position;
    } else {
        // Attacking the castle: shoot in facing direction
        diff = Engine::Point((direction == 1 ? -1 : 1), 0);  // +x or -x
    }

    Engine::Point normalized = diff.Normalize();
    rotation = atan2(normalized.y, normalized.x);
    Engine::Point normal = Engine::Point(-normalized.y, normalized.x);

    getPlayScene()->BulletGroup->AddNewObject(
        new Arrow(Position + normalized * 36 - normal * 6, diff, rotation, this)
    );
}