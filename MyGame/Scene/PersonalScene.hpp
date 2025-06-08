#ifndef INC_2025_I2P2_TOWERDEFENSE_WITH_ANSWER_STARTSCENE_H
#define INC_2025_I2P2_TOWERDEFENSE_WITH_ANSWER_STARTSCENE_H

#include <allegro5/allegro_audio.h>
#include <memory>

#include "Engine/IScene.hpp"
class PersonalScene final : public Engine::IScene {
private:
    bool host = false;
    bool waitConn = false;
public:
    explicit PersonalScene() = default;
    void Initialize() override;
    void Terminate() override;
    void Update(float dt) override;
    void HostGame();
    void JoinGame();
    void ScoreOnClick();
};
#endif   // INC_2025_I2P2_TOWERDEFENSE_WITH_ANSWER_STARTSCENE_H
