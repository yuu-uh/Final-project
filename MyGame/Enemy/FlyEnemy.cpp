#include <string>

#include "FlyEnemy.hpp"
#include "Bullet/Bullet.hpp"
#include "Enemy.hpp"
#include "Engine/Point.hpp"
#include "Scene/PlayScene.hpp"

FlyEnemy::FlyEnemy(int x, int y) : Enemy("play/enemy-4.png", x, y, 10, 20, 5, 5) {
}

void FlyEnemy::Update(float deltaTime){
    Engine::Point dir = (PlayScene::EndGridPoint * PlayScene::BlockSize + Engine::Point(PlayScene::BlockSize / 2, PlayScene::BlockSize / 2) - Position).Normalize();
    Velocity = dir * speed;
    Rotation = atan2(dir.y, dir.x);
    Sprite::Update(deltaTime);

    if ((Position - (PlayScene::EndGridPoint * PlayScene::BlockSize + Engine::Point(PlayScene::BlockSize / 2, PlayScene::BlockSize / 2))).Magnitude() < 5.0f) {
        Hit(hp);
        getPlayScene()->Hit();
    }
}
