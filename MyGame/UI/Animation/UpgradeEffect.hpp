#ifndef UPGRADEEFFECT_HPP
#define UPGRADEEFFECT_HPP
#include <allegro5/bitmap.h>
#include <memory>
#include <vector>

#include "Engine/Sprite.hpp"

class PlayScene;

class UpgradeEffect : public Engine::Sprite {
protected:
    PlayScene *getPlayScene();
    float timeTicks;
    std::vector<std::shared_ptr<ALLEGRO_BITMAP>> bmps;
    float timeSpan = 0.5;

public:
    UpgradeEffect(float x, float y);
    void Update(float deltaTime) override;
};
#endif   // EXPLOSIONEFFECT_HPP
