#include <string>

#include "dragen.hpp"
#include "Scene/PlayScene.hpp"
#include "Bullet/FireBullet.hpp"

Dragen::Dragen(int x, int y, int dir, bool isLocal):
Soldier("mapScene/dragen.png", x, y, dir ,30, 50, 100, 10, true){
    attackRadius = 150;
    cooldown = 0.5f;
};

std::string Dragen::soldierName() const{
    return "dragen";
}

void Dragen::CreateWeapon() {
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
        new FireBullet(Position + normalized * 36 - normal * 6, diff, rotation, this)
    );

}
