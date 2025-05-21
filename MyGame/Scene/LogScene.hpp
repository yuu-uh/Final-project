#ifndef LOGSCENE_HPP
#define LOGSCENE_HPP
#include <allegro5/allegro_audio.h>
#include <memory>

#include "Engine/IScene.hpp"
#include "UI/Component/Label.hpp"

class LogScene final : public Engine::IScene {
private:
    Engine::Label* nameLabel;
    Engine::Label* passwordLabel;
    std::string name;
    std::string password;
    bool editingName = true;  

public:
    explicit LogScene() = default;
    void Initialize() override;
    void Terminate() override;
    virtual void OnKeyDown(int keyCode) override;
    void OnKeyDownP(int keyCode);
};

#endif   
