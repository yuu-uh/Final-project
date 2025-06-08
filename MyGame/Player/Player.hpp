#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <string>
#include "Engine/Sprite.hpp"
#include "Engine/Point.hpp"

class MapScene;

class Player : public Engine::Sprite {
public:
    explicit Player(const std::string &imagePath,
                    const Engine::Point &startPos,
                    float moveSpeed,
                    int frameW = 16,
                    int frameH = 16);

    void Update(float deltaTime) override;
    void Draw() const override;
    std::unordered_map<std::string, std::vector<ALLEGRO_BITMAP*>> animations;
    uint8_t CurrentAction() const { return action; }
    void SetAction(uint8_t a);

protected:
    float speed;
    MapScene *playScene;

    virtual void OnCollision(Engine::Sprite *other);

private:
    uint8_t action = 0;
    float animationTimer = 0.0f;
    float frameDuration = 0.15f; // seconds per frame
    int cur_frame = 0;
    std::string cur_dir = "front";
    bool moving = false;
    bool CheckCollision(const Engine::Point &newPos);
    Engine::Point ReadInput();
    void LoadAnimations();
};

#endif // PLAYER_HPP
