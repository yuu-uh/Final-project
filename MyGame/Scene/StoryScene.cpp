#include "StoryScene.hpp"
#include <allegro5/allegro_audio.h>
#include <fstream>
#include <functional>
#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Resources.hpp"
#include "UI/Component/Image.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/ImageButton.hpp"

void StoryScene::Initialize() {
    cur_line = text_idx = 0; text_timer = auto_timer = 0.f; show_text.clear();
    char_interval = 0.05f;  
    std::ifstream infile("Resource/Story.txt"); std::string line;
    while (std::getline(infile, line)) lines.push_back(line);
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    textLabel = new Engine::Label("", "pirulen.ttf", 36, 70, h/2, 255,255,255,255, 0.0f,0.0f);
    //AddNewObject(textLabel);
    skipButton = new Engine::ImageButton("GUI/Skip.png","GUI/Skip.png", w-160, h-120, 80, 80);
    skipButton->SetOnClickCallback(std::bind(&StoryScene::TBOnClick, this, 0));
    AddNewControlObject(skipButton);
    bgmId = AudioHelper::PlayBGM("others.ogg");
}

void StoryScene::Update(float deltaTime) {
    if (cur_line < (int)lines.size()) {
        if (text_idx < (int)lines[cur_line].size()) {
            text_timer += deltaTime;
            if (text_timer >= char_interval) {
                show_text.push_back(lines[cur_line][text_idx++]);
                textLabel->SetText(show_text);
                text_timer -= char_interval;
            }
        } else {
            cur_line++;
            if (cur_line < (int)lines.size()) {
                show_text.clear();
                text_idx = 0;
                textLabel->SetText("");
            }
        }
    } else {
        Engine::GameEngine::GetInstance().ChangeScene("personal");
    }
}

void StoryScene::Draw() const {
    IScene::Draw();
    
    ALLEGRO_COLOR white = al_map_rgb(255,255,255);
    float x = textLabel->Position.x;
    float y = textLabel->Position.y;
    float maxW = Engine::GameEngine::GetInstance().GetScreenSize().x - x - 50;
    auto font = Engine::Resources::GetInstance().GetFont("pirulen.ttf", 36).get();
    int lineH = al_get_font_line_height(font);

    std::string s = show_text;
    int row = 0;
    while (!s.empty()) {
        int cut = s.size();
        while (cut > 0 && al_get_text_width(font, s.substr(0, cut).c_str()) > maxW)
            --cut;
        std::string part = s.substr(0, cut);
        al_draw_text(font, white, x, y + row * lineH, 0, part.c_str());
        s.erase(0, cut);
        ++row;
    }
}

void StoryScene::Terminate() {
    IScene::Terminate();
}

void StoryScene::advance_line() {
    if (cur_line + 1 < (int)lines.size()) {
        cur_line++; show_text.clear(); text_idx = 0; textLabel->SetText("");
    } else {
        cur_line = lines.size();
    }
}

void StoryScene::TBOnClick(int) {
    Engine::GameEngine::GetInstance().ChangeScene("personal");
}
