//
// Created by Hsuan on 2024/4/10.
//

#ifndef INC_2025_I2P2_TOWERDEFENSE_WITH_ANSWER_STARTSCENE_H
#define INC_2025_I2P2_TOWERDEFENSE_WITH_ANSWER_STARTSCENE_H

#include <allegro5/allegro_audio.h>
#include <memory>

#include "Engine/IScene.hpp"
class PersonalScene final : public Engine::IScene {
public:
    explicit PersonalScene() = default;
    void Initialize() override;
    void Terminate() override;
};
#endif   // INC_2025_I2P2_TOWERDEFENSE_WITH_ANSWER_STARTSCENE_H
