#include <string>

#include "vikin.hpp"
#include "Scene/PlayScene.hpp"
#include "Bullet/Sword.hpp"

Vikin::Vikin(int x, int y, int dir, bool isLocal):
Soldier("mapScene/vikin.png", x, y, dir, 10, 50, 150, 25, true){
    attackRadius = 50;
    cooldown = 0.5f;
};

std::string Vikin::soldierName() const{
    return "vikin";
}


void Vikin::CreateWeapon() {
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
        new Sword(Position + normalized * 10, diff, rotation, this)
    );
}