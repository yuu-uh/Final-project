#ifndef MAPSCENE_HPP
#define MAPSCENE_HPP
#include <allegro5/allegro_audio.h>
#include <memory>

#include "Engine/IScene.hpp"
#include "UI/Component/Label.hpp"

class MapScene final : public Engine::IScene {
private: 
    float remainTime = 0.0f;

public:
    explicit MapScene() = default;
    void Initialize() override;
    void ReadMap();
    void Update(float deltaTime) override;
    void Terminate() override;
};

#endif   
