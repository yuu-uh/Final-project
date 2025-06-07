#ifndef STORYSCENE_HPP
#define STORYSCENE_HPP
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <memory>

#include "Engine/IScene.hpp"

class StoryScene final : public Engine::IScene {
private:
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> bgmInstance;
    std::vector<std::string> lines;
    std::string show_text;
    int cur_line;
    int text_idx;
    float text_timer;
    float auto_timer;
    bool auto_mode;
    Group* bgGroup;
    Group* textGroup;
    ALLEGRO_RECT 

    bool mouse_in_rect(float x, float y, ALLEGRO_RECT r);

public:
    explicit StoryScene() = default;
    void Initialize() override;
    void Terminate() override;
    void Draw() const override;
    void Update(float deltaTime) override;
    void AutoOnClick(int stage);
    void TBOnClick(int stage);
    void advance_line();
};

#endif   // STAGESELECTSCENE_HPP
