#ifndef INC_2025_I2P2_TOWERDEFENSE_WITH_ANSWER_STARTSCENE_H
#define INC_2025_I2P2_TOWERDEFENSE_WITH_ANSWER_STARTSCENE_H

#include <allegro5/allegro_audio.h>
#include <memory>
#include "UI/Component/TextBox.hpp"
#include "UI/Component/TextBox.hpp"
#include "Engine/IScene.hpp"
class PersonalScene final : public Engine::IScene {
private:
    std::shared_ptr<ALLEGRO_FONT> font;
    bool host = false;
    bool waitConn = false;
    std::string _peerIp = "192.168.101.233";
    bool join_mode = false;
    Engine::TextBox* IPEnter = nullptr;
    Engine::TextBox* portEenter = nullptr;
    Engine::TextBox* focusedBox = nullptr;

public:
    explicit PersonalScene() = default;
    void Initialize() override;
    void Terminate() override;
    void Update(float dt) override;
    void HostGame();
    void JoinGame();
    void ScoreOnClick();
    void OnKeyDown(int keycode) override;
    void Draw() const override;
    void ConfirmJoin();
    void OnKeyDown(int keycode) override;
    void Draw() const override;
    void ConfirmJoin();
};
#endif   // INC_2025_I2P2_TOWERDEFENSE_WITH_ANSWER_STARTSCENE_H
