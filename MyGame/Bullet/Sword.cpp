// Sword.cpp
#include "Sword.hpp"
#include "Engine/Collider.hpp"
#include "Scene/PlayScene.hpp"

Sword::Sword(Engine::Point position, Engine::Point forwardDirection, float rotation, Soldier* parent)
    : Bullet("mapScene/Ammo1.png", 200, 1, position, forwardDirection, rotation, parent), lifeTime(0.2), elapsed(0) {
    CollisionRadius = 25; // Sword has large area of effect
    Velocity = Engine::Point(0, 0);   // Sword doesn't travel
}

void Sword::Update(float deltaTime) {
    elapsed += deltaTime;

    PlayScene* scene = getPlayScene();
    if (!scene) return;

    // Check collision with enemies
    for (auto& it : parent->Enemy->GetObjects()) {
        Soldier* enemy = dynamic_cast<Soldier*>(it);
        if (!enemy->Visible) continue;
        if (Engine::Collider::IsCircleOverlap(Position, CollisionRadius, enemy->Position, enemy->CollisionRadius)) {
            OnExplode(enemy);
        }
    }

    if (elapsed >= lifeTime) {
        getPlayScene()->BulletGroup->RemoveObject(objectIterator);
    }
}

// void Sword::OnExplode(Soldier* enemy) {
//     //if (!enemy) return;
//     //enemy->Hit(damage); // Deal damage
//     //getPlayScene()->EffectGroup->AddNewObject(new ExplosionEffect(enemy->Position.x, enemy->Position.y));
// }
