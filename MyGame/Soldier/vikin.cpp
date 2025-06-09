#include <string>

#include "vikin.hpp"
#include "Scene/PlayScene.hpp"
#include "Bullet/FireBullet.hpp"

Vikin::Vikin(int x, int y, int dir, bool isLocal):
Soldier("mapScene/vikin.png", x, y, dir, 10, 50, 100, 10, true){
    attackRadius = 100;
    cooldown = 0.5f;
};

std::string Vikin::soldierName() const{
    return "vikin";
}


void Vikin::CreateWeapon() {
    Engine::Point diff = Engine::Point(cos(Rotation - ALLEGRO_PI / 2), sin(Rotation - ALLEGRO_PI / 2));
    float rotation = atan2(diff.y, diff.x);
    Engine::Point normalized = diff.Normalize();
    Engine::Point normal = Engine::Point(-normalized.y, normalized.x);
    getPlayScene()->BulletGroup->AddNewObject(new FireBullet(Position + normalized * 36 - normal * 6, diff, rotation, this));
}