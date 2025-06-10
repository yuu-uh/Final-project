#ifndef LOGSCENE_HPP
#define LOGSCENE_HPP
#include <allegro5/allegro_audio.h>
#include <memory>

#include "Engine/IScene.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/TextBox.hpp"

class LogScene final : public Engine::IScene {
private: 
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance;
    ALLEGRO_SAMPLE_ID bgmId;
    Engine::Label* nameLabel;
    Engine::Label* passwordLabel;
    std::string name;
    std::string password;
    bool editingName = true;  

public:
    Engine::TextBox* Enter1 = nullptr;
    Engine::TextBox* Enter2 = nullptr;
    explicit LogScene() = default;
    void Initialize() override;
    void Terminate() override;
    virtual void OnKeyDown(int keyCode) override;
    void OnKeyDownP(int keyCode);
};

#endif   
