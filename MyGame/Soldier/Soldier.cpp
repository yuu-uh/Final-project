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
#include "UI/Animation/ExplosionEffect.hpp"


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

Soldier::Soldier(std::string img, float x, float y, int dir, float radius, float speed, float hp, float damage, bool isLocal) : Engine::Sprite(img, x, y, 60, 60, 0, 0, 0, 10, 10), speed(speed), hp(hp), dmg(damage) {
    CollisionRadius = radius;
    reachEndTime = 0;
    maxHp =  hp;
    direction = dir;
    state = walking;
    PlayScene* scene = getPlayScene();
    this->baseY = y;
    Enemy = (direction == 1) ? scene->EnemyGroup:scene->SoldierGroup;
}

void Soldier::Hit(float damage) {
    hp -= damage;
    getPlayScene()->EffectGroup->AddNewObject(new ExplosionEffect(Position.x, Position.y));
    if (hp <= 0) {
        die();
        //AudioHelper::PlayAudio("explosion.wav");
        PlayScene* scene = getPlayScene();

        // Nullify all Soldiers targeting this one
        for (auto& it : scene->SoldierGroup->GetObjects()) {
            Soldier* soldier = dynamic_cast<Soldier*>(it);
            if (soldier && soldier->target == this) {
                soldier->target = nullptr;
                soldier->state = walking;
            }
        }
        for (auto& it : scene->EnemyGroup->GetObjects()) {
            Soldier* soldier = dynamic_cast<Soldier*>(it);
            if (soldier && soldier->target == this) {
                soldier->target = nullptr;
                soldier->state = walking;
            }
        }

        if (direction == 1) {
            scene->SoldierGroup->RemoveObject(objectIterator);  // Left-to-right → Player
        } else {
            scene->EnemyGroup->RemoveObject(objectIterator);    // Right-to-left → Enemy
        }
    }
}

void Soldier::Attack() {
    PlayScene* scene = getPlayScene();
    if (target)
        target->Hit(dmg);
    else{
        if (direction == 1) {
                    // Player's soldier attacking enemy castle
            scene->SendCastleDamage(1);  // Send network message
        } else {
                    // Enemy soldier attacking player's castle
            scene->Hit();  // Damage local player
        }
    } // Default: melee attack (sword)
}

void Soldier::Update(float deltaTime){
    if(Preview) return;
    
    PlayScene* scene = getPlayScene();
    
    if (!scene) return;

    int tileX = floor(Position.x / scene->BlockSize);
    int tileY = floor(Position.y / scene->BlockSize);
    if (tileX >= 0 && tileX < scene->MapWidth && tileY >= 0 && tileY < scene->MapHeight) {
        if (scene->mapData[tileY][tileX] == 4) {
            // Attack the castle (or set a dummy target)
            // Example: just stop moving and simulate attack cooldown
            state = attacking;
            target = nullptr;
            attackTimer -= deltaTime;
            if (attackTimer <= 0) {
                attackTimer = cooldown;
                Attack();
            }
            return;
        }
    }

    if (state == walking && !target) {
        Position.x -= speed * direction * deltaTime;
        float_timer += deltaTime;
        int tmpY = Position.y;
        Position.y = tmpY + (std::sin(float_timer * 4.0f) / 6); // vertical oscillation
        Position.y = tmpY;
    }

    // Look for enemy if no target
    if (!target) {
        for (auto& it : Enemy->GetObjects()) {
            Soldier* enemy = dynamic_cast<Soldier*>(it);
            if (!enemy) continue;
            Engine::Point diff = enemy->Position - Position;
            if (diff.Magnitude() <= attackRadius) {
                target = enemy;
                state = attacking;
                break;
            }
        }
    }

    // If has a target
    if (target) {
        Engine::Point diff = target->Position - Position;
        if (diff.Magnitude() > attackRadius || !target->Enabled) {
            // Lost target
            target = nullptr;
            state = walking;
        } else {
            // Attack target
            attackTimer -= deltaTime;
            if (attackTimer <= 0) {
                Attack();
                attackTimer = cooldown;
            }
        }
    }

    // Check if off-screen
    if (Position.x < 0 || Position.x >= scene->BlockSize * scene->MapWidth) {
        getPlayScene()->SoldierGroup->RemoveObject(objectIterator);
    }
}


void Soldier::Draw() const {
    // if(Preview){
    //     al_draw_filled_circle(Position.x, Position.y, CollisionRadius, al_map_rgba(0, 255, 0, 50));
    // }
    Sprite::Draw();
    if(Preview) return;
    float barWidth = 40.0f;
    float barHeight = 5.0f;
    float hpPercentage = hp / maxHp;
    float barX = Position.x;
    float barY = Position.y;

    al_draw_filled_rectangle(barX, barY, barX + barWidth, barY + barHeight, al_map_rgb(255, 0, 0));
    al_draw_filled_rectangle(barX, barY, barX + barWidth * hpPercentage, barY + barHeight, al_map_rgb(0, 255, 0));
    al_draw_rectangle(barX, barY, barX + barWidth, barY + barHeight, al_map_rgb(0, 0, 0), 1);
    // if (PlayScene::DebugMode) {
    //     // Draw collision radius.
    //     al_draw_circle(Position.x, Position.y, CollisionRadius, al_map_rgb(255, 0, 0), 2);
    // }
}
