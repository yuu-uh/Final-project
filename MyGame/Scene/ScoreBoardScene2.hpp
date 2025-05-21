#ifndef ScoreBoardScene2_HPP
#define ScoreBoardScene2_HPP
#include <memory>

#include "Engine/IScene.hpp"
#include <allegro5/allegro_audio.h>

class ScoreBoardScene2 final : public Engine::IScene {
public:
    explicit ScoreBoardScene2() = default;
    void Initialize() override;
    void Terminate() override;
    void BackOnClick(int stage);
    void PrevOnClick();
};

#endif   
