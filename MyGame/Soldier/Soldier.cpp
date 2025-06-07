#include <allegro5/allegro_primitives.h>
#include <allegro5/color.h>
#include <cmath>
#include <random>
#include <string>
#include <vector>

#include "Bullet/Bullet.hpp"
#include "Soldier.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "Engine/IScene.hpp"
#include "Engine/LOG.hpp"
#include "Scene/PlayScene.hpp"
//#include "UI/Animation/DirtyEffect.hpp"
//#include "UI/Animation/ExplosionEffect.hpp"


PlayScene *Soldier::getPlayScene() {
    return dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetActiveScene());
}
void Soldier::die() {
    //getPlayScene()->EffectGroup->AddNewObject(new ExplosionEffect(Position.x, Position.y));
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> distId(1, 3);
    std::uniform_int_distribution<std::mt19937::result_type> dist(1, 20);
    for (int i = 0; i < 10; i++) {
        // Random add 10 dirty effects.
        //getPlayScene()->GroundEffectGroup->AddNewObject(new DirtyEffect("play/dirty-" + std::to_string(distId(rng)) + ".png", dist(rng), Position.x, Position.y));
    }
}
Soldier::Soldier(std::string img, float x, float y, float radius, float speed, float hp, float damage) : Engine::Sprite(img, x, y, 60, 60, 0, 0, 0, 10, 10), speed(speed), hp(hp), dmg(damage) {
    CollisionRadius = radius;
    reachEndTime = 0;
}
void Soldier::Hit(float damage) {
    hp -= damage;
    if (hp <= 0) {
        die();
        // Remove all turret's reference to target.

        getPlayScene()->SoldierGroup->RemoveObject(objectIterator);
        AudioHelper::PlayAudio("explosion.wav");
    }
}
void Soldier::UpdatePath(const std::vector<std::vector<int>> &mapDistance) {
    int x = static_cast<int>(floor(Position.x / PlayScene::BlockSize));
    int y = static_cast<int>(floor(Position.y / PlayScene::BlockSize));
    if (x < 0) x = 0;
    if (x >= PlayScene::MapWidth) x = PlayScene::MapWidth - 1;
    if (y < 0) y = 0;
    if (y >= PlayScene::MapHeight) y = PlayScene::MapHeight - 1;
    Engine::Point pos(x, y);
    int num = mapDistance[y][x];
    if (num == -1) {
        num = 0;
        Engine::LOG(Engine::ERROR) << "Enemy path finding error";
    }
    path = std::vector<Engine::Point>(num + 1);
    while (num != 0) {
        std::vector<Engine::Point> nextHops;
        for (auto &dir : PlayScene::directions) {
            int x = pos.x + dir.x;
            int y = pos.y + dir.y;
            if (x < 0 || x >= PlayScene::MapWidth || y < 0 || y >= PlayScene::MapHeight || mapDistance[y][x] != num - 1)
                continue;
            nextHops.emplace_back(x, y);
        }
        
        // Choose arbitrary one.
        std::random_device dev;
        std::mt19937 rng(dev());
        std::uniform_int_distribution<std::mt19937::result_type> dist(0, nextHops.size() - 1);
        pos = nextHops[dist(rng)];
        path[num] = pos;
        num--;
    }
    //path[0] = PlayScene::EndGridPoint;
}
void Soldier::Update(float deltaTime){
    // Pre-calculate the velocity.
    //Sprite::Update(deltaTime);

    if(state != walking)
        return;

    Position.x -= speed * deltaTime;
    float_timer += deltaTime;
    Position.y += (std::sin(float_timer*4.0f) / 6); // oscillates ±5px


    // If off-screen or reached the opponent's base (x <= 0), remove soldier.
    if (Position.x < 0) {
        getPlayScene()->SoldierGroup->RemoveObject(objectIterator);
        // You might also reduce player HP here if needed.
    }
    
}
void Soldier::Draw() const {
    Sprite::Draw();
    // if (PlayScene::DebugMode) {
    //     // Draw collision radius.
    //     al_draw_circle(Position.x, Position.y, CollisionRadius, al_map_rgb(255, 0, 0), 2);
    // }
}
