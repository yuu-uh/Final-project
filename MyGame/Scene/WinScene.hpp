#ifndef WINSCENE_HPP
#define WINSCENE_HPP
#include "Engine/IScene.hpp"
#include <allegro5/allegro_audio.h>

class WinScene final : public Engine::IScene {
private:
    float ticks;
    std::string playerName;        
    Engine::Label* nameLabel;
    ALLEGRO_SAMPLE_ID bgmId;
    int score;

public:
    explicit WinScene() = default;
    void Initialize() override;
    void Terminate() override;
    void Update(float deltaTime) override;
    void BackOnClick(int stage);
    virtual void OnKeyDown(int keyCode) override;
    void SetScore(int s){ score = s; }
};

#endif   // WINSCENE_HPP
