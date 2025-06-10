#include <allegro5/base.h>
#include <random>
#include <string>

#include "Soldier/Soldier.hpp"
#include "Engine/Group.hpp"
#include "Engine/Point.hpp"
#include "Knife.hpp"
#include "Scene/PlayScene.hpp"
#include "UI/Animation/DirtyEffect.hpp"

class Turret;

Knife::Knife(Engine::Point position, Engine::Point forwardDirection, float rotation, Soldier *parent) : Bullet("mapScene/nife.png", 500, 1, position, forwardDirection, rotation - ALLEGRO_PI / 2, parent) {
}
void Knife::OnExplode(Soldier *enemy) {
    //enemy->setSpeed(enemy->getSpeed()*0.5);
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(2, 5);
    //getPlayScene()->GroundEffectGroup->AddNewObject(new DirtyEffect("play/dirty-1.png", dist(rng), enemy->Position.x, enemy->Position.y));
}
