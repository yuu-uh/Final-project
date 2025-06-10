#include <allegro5/base.h>
#include <random>
#include <string>

#include "Soldier/Soldier.hpp"
#include "Engine/Group.hpp"
#include "Engine/Point.hpp"
#include "LaserBullet.hpp"
#include "Scene/PlayScene.hpp"
#include "UI/Animation/DirtyEffect.hpp"

class Soldier;

LaserBullet::LaserBullet(Engine::Point position, Engine::Point forwardDirection, float rotation) : Bullet("mapScene/bullet-2.png", 300, 2, position, forwardDirection, rotation - ALLEGRO_PI / 2, nullptr) {
}
void LaserBullet::OnExplode(Soldier *Soldier) {
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(2, 10);
    //getPlayScene()->GroundEffectGroup->AddNewObject(new DirtyEffect("play/dirty-2.png", dist(rng), Soldier->Position.x, Soldier->Position.y));
}
