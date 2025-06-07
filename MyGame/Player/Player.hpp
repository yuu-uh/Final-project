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

protected:
    float speed;
    MapScene *playScene;

    virtual void OnCollision(Engine::Sprite *other);

private:
    bool CheckCollision(const Engine::Point &newPos);
    Engine::Point ReadInput();
};

#endif // PLAYER_HPP
