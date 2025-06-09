#include <string>

#include "slime.hpp"
#include "Scene/PlayScene.hpp"
#include "Bullet/FireBullet.hpp"

Slime::Slime(int x, int y, int dir, bool isLocal):
Soldier("mapScene/slime.png", x, y, dir, 10, 50, 100, 10, true){
    attackRadius = 100;
    cooldown = 0.5f;
};

std::string Slime::soldierName() const{
    return "slime";
}


void Slime::CreateWeapon() {
    Engine::Point diff = Engine::Point(cos(Rotation - ALLEGRO_PI / 2), sin(Rotation - ALLEGRO_PI / 2));
    float rotation = atan2(diff.y, diff.x);
    Engine::Point normalized = diff.Normalize();
    Engine::Point normal = Engine::Point(-normalized.y, normalized.x);
    getPlayScene()->BulletGroup->AddNewObject(new FireBullet(Position + normalized * 36 - normal * 6, diff, rotation, this));
}