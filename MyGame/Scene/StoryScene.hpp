#pragma once
#include <vector>
#include <string>
#include "Engine/IScene.hpp"
#include "Engine/AudioHelper.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp

class StoryScene : public Engine::IScene {
public:
    void Initialize() override;
    void Update(float deltaTime) override;
    void Draw() const override;
    void Terminate() override;
private:
    void advance_line();
    void TBOnClick(int);
    ALLEGRO_SAMPLE_ID bgmId;
    std::vector<std::string> lines;
    std::string show_text;
    int cur_line = 0, text_idx = 0;
    float text_timer = 0.f, auto_timer = 0.f, char_interval = 0.05f;
    Engine::Label* textLabel = nullptr;
    Engine::ImageButton* skipButton = nullptr;
};
