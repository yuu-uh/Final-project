#ifndef RESULTSCENE_HPP
#define RESULTSCENE_HPP
#include "Engine/IScene.hpp"
#include "Engine/Group.hpp"
#include <allegro5/allegro_audio.h>

class ResultScene final : public Engine::IScene {
private:
    float ticks;
    std::string playerName;        
    Engine::Label* nameLabel;
    ALLEGRO_SAMPLE_ID bgmId;
    int score;
    Group* UIGroup;

public:
    explicit ResultScene() = default;
    void Initialize() override;
    void Terminate() override;
    void Update(float deltaTime) override;
    void BackOnClick(int stage);
};

#endif   // WINSCENE_HPP
