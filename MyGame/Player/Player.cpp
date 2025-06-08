#include "Player.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Resources.hpp"
#include "Engine/LOG.hpp"
#include "Scene/MapScene.hpp"
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>

void Player::LoadAnimations(){
    const std::vector<std::string> directions = {"front", "back", "left", "right"};
    for (const auto& dir : directions) {
        std::vector<ALLEGRO_BITMAP*> frames;
        for (int i = 1; i <= 4; i++) {
            std::string path = "Resource/images/mapScene/player1_" + dir + "0" + std::to_string(i) + ".png";
            ALLEGRO_BITMAP* bmp = al_load_bitmap(path.c_str());
            if (!bmp) {
                Engine::LOG(Engine::INFO) << "Failed to load: " << path;
                continue;
            }
            frames.push_back(bmp);
        }
        animations[dir] = frames;
    }
    Engine::LOG(Engine::INFO)<<"animation loaded";
}

Player::Player(const std::string &imagePath, const Engine::Point &startPos, float moveSpeed,
               int frameW, int frameH, bool isLocal)
    : Engine::Sprite(
          imagePath,
          static_cast<float>(startPos.x),
          static_cast<float>(startPos.y),
          60, 60
      ),
      speed(moveSpeed), isLocal(isLocal)
{
    playScene = dynamic_cast<MapScene*>(
        Engine::GameEngine::GetInstance().GetScene("map")
    );
    CollisionRadius = 50;
    Engine::LOG(Engine::INFO)<<"start to load animations";
    LoadAnimations();
}
void Player::SetAction(uint8_t a){
    action = a;
    switch (a) {
        case 0: cur_dir = "front"; moving = false; break;
        case 1: cur_dir = "back";  moving = true;  break;
        case 2: cur_dir = "left";  moving = true;  break;
        case 3: cur_dir = "right"; moving = true;  break;
        default: cur_dir = "front"; moving = false; break;
    }
}
void Player::Update(float deltaTime) {
    Engine::Point dir{0, 0};
    if(isLocal) dir = ReadInput();
    if (dir.x || dir.y) {
        Engine::Point np = Position;
        np.x += dir.x * speed * deltaTime;
        np.y += dir.y * speed * deltaTime;
        if (!CheckCollision(np))
            Position = np;
        else
            OnCollision(nullptr);
    }
    if (moving && !animations[cur_dir].empty()) {
        animationTimer += deltaTime;
        if (animationTimer >= frameDuration) {
            animationTimer -= frameDuration;
            cur_frame = (cur_frame + 1) % animations[cur_dir].size();
        }
    } else {
        cur_frame = 0; // Idle pose
    }
    Engine::Sprite::Update(deltaTime);
}

bool Player::CheckCollision(const Engine::Point &newPos) {
    int gx = static_cast<int>(newPos.x) / MapScene::BlockSize;
    int gy = static_cast<int>(newPos.y) / MapScene::BlockSize;
    //return !playScene->CheckSpaceValid(gx, gy);
    return false;
}

void Player::Draw() const{
    if (!animations.empty() && !animations.at(cur_dir).empty() && animations.at(cur_dir).at(0)) {
        ALLEGRO_BITMAP* frame = animations.at(cur_dir).at(cur_frame);
        int bmpW = al_get_bitmap_width(frame);
        int bmpH = al_get_bitmap_height(frame);

        al_draw_scaled_bitmap(
            frame,
            0, 0,           // source top-left
            bmpW, bmpH,     // source width, height
            Position.x, Position.y,  // destination top-left
            60, 60,         // destination width, height (scale here)
            0               // flags
        );
    } else {
        Engine::Sprite::Draw();  // fallback
    }
}

Engine::Point Player::ReadInput() {
    ALLEGRO_KEYBOARD_STATE ks;
    al_get_keyboard_state(&ks);
    Engine::Point d{0,0};
    moving = false;
    if (al_key_down(&ks, ALLEGRO_KEY_UP)) {
        d.y = -1;
        moving = true;
        cur_dir = "back";
    }   
    if (al_key_down(&ks, ALLEGRO_KEY_DOWN)){
        d.y = +1;
        moving = true;
        cur_dir = "front";
    }  
    if (al_key_down(&ks, ALLEGRO_KEY_LEFT)) {
        d.x = -1;
        moving = true;
        cur_dir = "left";
    } 
    if (al_key_down(&ks, ALLEGRO_KEY_RIGHT)){
        d.x = +1;
        moving = true;
        cur_dir = "right";
    } 
    return d;
}

void Player::OnCollision(Engine::Sprite *other) {
    // no-op
}
