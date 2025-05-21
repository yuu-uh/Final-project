#ifndef ScoreBoardScene1_HPP
#define ScoreBoardScene1_HPP
#include <memory>

#include "Engine/IScene.hpp"
#include <allegro5/allegro_audio.h>

class ScoreBoardScene1 final : public Engine::IScene {
public:
    explicit ScoreBoardScene1() = default;
    void Initialize() override;
    void Terminate() override;
    void BackOnClick(int stage);
    void NextOnClick();
};

#endif   
