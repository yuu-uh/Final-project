#include <string>

#include "ninja.hpp"
#include "Scene/PlayScene.hpp"
#include "Bullet/Knife.hpp"

Ninja::Ninja(int x, int y, int dir, bool isLocal):
Soldier("mapScene/ninja.png", x, y, dir, 10, 50, 100, 10, true){
    attackRadius = 100;
    cooldown = 0.5f;
};

std::string Ninja::soldierName() const{
    return "ninja";
}


void Ninja::CreateWeapon() {
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
        new Knife(Position + normalized * 10, diff, rotation, this)
    );
}