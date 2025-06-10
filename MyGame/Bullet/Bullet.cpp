#include "Bullet.hpp"
#include "Engine/Collider.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "Engine/IObject.hpp"
#include "Engine/IScene.hpp"
#include "Engine/Point.hpp"
#include "Engine/Sprite.hpp"
#include "Scene/PlayScene.hpp"

PlayScene *Bullet::getPlayScene() {
    return dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetActiveScene());
}
void Bullet::OnExplode(Soldier *enemy) {
}
Bullet::Bullet(std::string img, float speed, float damage, Engine::Point position, Engine::Point forwardDirection, float rotation, Soldier *parent) : Sprite(img, position.x, position.y), speed(speed), damage(damage), parent(parent) {
    Velocity = forwardDirection.Normalize() * speed;
    Rotation = rotation;
    CollisionRadius = 1;
}
void Bullet::Update(float deltaTime) {
    Sprite::Update(deltaTime);
    PlayScene *scene = getPlayScene();

    // === Check collision with enemies ===
    for (auto &it : parent->Enemy->GetObjects()) {
        Soldier *enemy = dynamic_cast<Soldier *>(it);
        if (!enemy->Visible)
            continue;
        if (Engine::Collider::IsCircleOverlap(Position, CollisionRadius, enemy->Position, enemy->CollisionRadius)) {
            OnExplode(enemy);
            scene->BulletGroup->RemoveObject(objectIterator);
            return;
        }
    }

    // === Check collision with tile (castle) ===
    int gridX = static_cast<int>(Position.x) / PlayScene::BlockSize;
    int gridY = static_cast<int>(Position.y) / PlayScene::BlockSize;

    if (gridX >= 0 && gridX < PlayScene::MapWidth &&
        gridY >= 0 && gridY < PlayScene::MapHeight) {

        int tileID = scene->mapData[gridY][gridX];
        if (tileID == 2 || tileID == 3 || tileID == 6) {
            // Bullet hits castle tile
            scene->BulletGroup->RemoveObject(objectIterator);
            return;
        }
    }

    // === Check if out of bounds ===
    if (!Engine::Collider::IsRectOverlap(Position - Size / 2, Position + Size / 2, Engine::Point(0, 0), PlayScene::GetClientSize()))
        getPlayScene()->BulletGroup->RemoveObject(objectIterator);
}

