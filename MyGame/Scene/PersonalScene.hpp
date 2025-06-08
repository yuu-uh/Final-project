#ifndef INC_2025_I2P2_TOWERDEFENSE_WITH_ANSWER_STARTSCENE_H
#define INC_2025_I2P2_TOWERDEFENSE_WITH_ANSWER_STARTSCENE_H

#include <allegro5/allegro_audio.h>
#include <memory>

#include "Engine/IScene.hpp"
class PersonalScene final : public Engine::IScene {
private:
    bool host = false;
    std::string _peerIp = "192.168.101.233";
public:
    explicit PersonalScene() = default;
    void Initialize() override;
    void Terminate() override;
    void HostGame();
    void JoinGame();
    void ScoreOnClick();
};
#endif   // INC_2025_I2P2_TOWERDEFENSE_WITH_ANSWER_STARTSCENE_H
