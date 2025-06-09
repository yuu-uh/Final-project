#include <string>

#include "master.hpp"
#include "Scene/PlayScene.hpp"
#include "Bullet/FireBullet.hpp"

Master::Master(int x, int y, int dir, bool isLocal):
Soldier("mapScene/master.png", x, y, dir, 10, 50, 100, 10, true){
    attackRadius = 100;
    cooldown = 0.5f;
};

std::string Master::soldierName() const{
    return "master";
}


void Master::CreateWeapon() {
    Engine::Point diff = Engine::Point(cos(Rotation - ALLEGRO_PI / 2), sin(Rotation - ALLEGRO_PI / 2));
    float rotation = atan2(diff.y, diff.x);
    Engine::Point normalized = diff.Normalize();
    Engine::Point normal = Engine::Point(-normalized.y, normalized.x);
    getPlayScene()->BulletGroup->AddNewObject(new FireBullet(Position + normalized * 36 - normal * 6, diff, rotation, this));
}
