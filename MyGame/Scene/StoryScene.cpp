#include <allegro5/allegro_audio.h>
#include <functional>
#include <memory>
#include <string>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "PlayScene.hpp"
#include "StoryScene.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/Slider.hpp"

void StoryScene::Initialize() {
    cur_line = 0, text_idx = 0, auto_timer = 0, text_timer = 0;
    auto_mode = false;
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;

    lines = {"Somewhere beyond the veil of this world lies the Mirror Realm.",
        "There, every soul has a reflection—equal in skill, opposite in heart.",
        "You have been summoned... not by choice, but by fate.",
        "To return, you must face your mirror and win—not just with strength, but with clarity.",
        "Before the duel, you and your rival will scavenge the same battleground for equipment.",
        "Only one will emerge... or perhaps, both will change forever." 
    };

    Engine::ImageButton *btn;

    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 200, halfH / 2 - 50, 400, 100);
    btn->SetOnClickCallback(std::bind(&StoryScene::AutoOnClick, this, 1));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Stage 1", "pirulen.ttf", 48, halfW, halfH / 2, 0, 0, 0, 255, 0.5, 0.5));
    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 200, halfH / 2 + 100, 400, 100);
    btn->SetOnClickCallback(std::bind(&StoryScene::TBOnClick, this, 2));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Stage 2", "pirulen.ttf", 48, halfW, halfH / 2 + 150, 0, 0, 0, 255, 0.5, 0.5));
    // Not safe if release resource while playing, however we only free while change scene, so it's fine.
    bgmInstance = AudioHelper::PlaySample("select.ogg", true, AudioHelper::BGMVolume);
}
void StoryScene::Terminate() {
    AudioHelper::StopSample(bgmInstance);
    bgmInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    IScene::Terminate();
}

void StoryScene::Update(float deltaTime){
    if(text_idx <= lines[cur_line].size()){
        text_timer += deltaTime;
        if(text_timer > 0.3){
            show_text += lines[cur_line][text_idx];
            text_timer = 0;
        }
    }
    if(auto_mode){
        auto_timer += deltaTime;
        if(auto_timer >= 2.0){
            advance_line();
            auto_timer = 0;
        }
    }
    if(cur_line >= lines.size()){
        
    }
}

void StoryScene::Draw() const{
    
    
}

void StoryScene::advance_line() {
    if (cur_line + 1 < lines.size()) {
        cur_line++;
        show_text = "";
        text_idx = 0;
    } else {
        cur_line++;
    }
}

void StoryScene::AutoOnClick(int stage) {
    
}
void StoryScene::TBOnClick(int stage) {
    
}

