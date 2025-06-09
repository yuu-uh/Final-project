#include <cmath>
#include <string>

#include "Engine/GameEngine.hpp"
#include "Engine/Group.hpp"
#include "Engine/IScene.hpp"
#include "Engine/Resources.hpp"
#include "UpgradeEffect.hpp"
#include "Scene/PlayScene.hpp"

PlayScene *UpgradeEffect::getPlayScene() {
    return dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetActiveScene());
}
UpgradeEffect::UpgradeEffect(float x, float y) : Sprite("play/light-6.png", x, y), timeTicks(0) {
    for (int i = 7; i <= 10; i++) {
        bmps.push_back(Engine::Resources::GetInstance().GetBitmap("play/light-" + std::to_string(i) + ".png"));
    }
}
void UpgradeEffect::Update(float deltaTime) {
    timeTicks += deltaTime;
    if (timeTicks >= timeSpan) {
        getPlayScene()->EffectGroup->RemoveObject(objectIterator);
        return;
    }
    int phase = floor(timeTicks / timeSpan * bmps.size());
    bmp = bmps[phase];
    Sprite::Update(deltaTime);
}
