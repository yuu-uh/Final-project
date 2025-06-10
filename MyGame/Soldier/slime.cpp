#include <string>

#include "slime.hpp"
#include "Scene/PlayScene.hpp"
#include "Bullet/rock.hpp"

Slime::Slime(int x, int y, int dir, bool isLocal):
Soldier("mapScene/slime.png", x, y, dir, 10, 50, 100, 10, true){
    attackRadius = 100;
    cooldown = 1.0f;
};

std::string Slime::soldierName() const{
    return "slime";
}


void Slime::CreateWeapon() {
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
        new Rock(Position + normalized * 36 - normal * 6, diff, rotation, this)
    );
}