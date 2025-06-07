#include "Player.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Resources.hpp"
#include "Scene/MapScene.hpp"
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>

Player::Player(const std::string &imagePath,
               const Engine::Point &startPos,
               float moveSpeed,
               int frameW,
               int frameH)
    : Engine::Sprite(
          imagePath,
          static_cast<float>(startPos.x),
          static_cast<float>(startPos.y),
          80, 80
      ),
      speed(moveSpeed)
{
    playScene = dynamic_cast<MapScene*>(
        Engine::GameEngine::GetInstance().GetScene("map")
    );
}

void Player::Update(float deltaTime) {
    Engine::Point dir = ReadInput();
    if (dir.x || dir.y) {
        Engine::Point np = Position;
        np.x += dir.x * speed * deltaTime;
        np.y += dir.y * speed * deltaTime;
        if (!CheckCollision(np))
            Position = np;
        else
            OnCollision(nullptr);
    }
    Engine::Sprite::Update(deltaTime);
}

bool Player::CheckCollision(const Engine::Point &newPos) {
    int gx = static_cast<int>(newPos.x) / MapScene::BlockSize;
    int gy = static_cast<int>(newPos.y) / MapScene::BlockSize;
    //return !playScene->CheckSpaceValid(gx, gy);
}

Engine::Point Player::ReadInput() {
    ALLEGRO_KEYBOARD_STATE ks;
    al_get_keyboard_state(&ks);
    Engine::Point d{0,0};
    if (al_key_down(&ks, ALLEGRO_KEY_UP))    d.y = -1;
    if (al_key_down(&ks, ALLEGRO_KEY_DOWN))  d.y = +1;
    if (al_key_down(&ks, ALLEGRO_KEY_LEFT))  d.x = -1;
    if (al_key_down(&ks, ALLEGRO_KEY_RIGHT)) d.x = +1;
    return d;
}

void Player::OnCollision(Engine::Sprite *other) {
    // no-op
}
