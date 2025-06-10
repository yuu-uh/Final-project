#include "StoryScene.hpp"
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_primitives.h>
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
    curBG = "background/battleback8.png";
    bgmId = AudioHelper::PlayBGM("others.ogg");
}

// void StoryScene::Update(float deltaTime) {
//     if (cur_line < (int)lines.size()) {
//         if (text_idx < (int)lines[cur_line].size()) {
//             text_timer += deltaTime;
//             if (text_timer >= char_interval) {
//                 show_text.push_back(lines[cur_line][text_idx++]);
//                 textLabel->SetText(show_text);
//                 text_timer -= char_interval;
//             }
//         } else {
//             cur_line++;
//             if (cur_line < (int)lines.size()) {
//                 show_text.clear();
//                 text_idx = 0;
//                 textLabel->SetText("");
//             }
//         }
//     } else {
//         Engine::GameEngine::GetInstance().ChangeScene("personal");
//     }
// }

void StoryScene::Update(float deltaTime) {
    if (cur_line < (int)lines.size()) {
        if (!lineDone) {
            if (text_idx < (int)lines[cur_line].size()) {
                text_timer += deltaTime;
                if (text_timer >= char_interval) {
                    show_text.push_back(lines[cur_line][text_idx++]);
                    textLabel->SetText(show_text);
                    text_timer -= char_interval;
                }
            } else {
                // Finished typing the line
                lineDone = true;
                pauseTimer = 0.0f;
            }
        } else {
            // Pause after full line is typed
            pauseTimer += deltaTime;
            if (pauseTimer >= pauseDuration) {
                cur_line++;
                if (cur_line < (int)lines.size()) {
                    show_text.clear();
                    text_idx = 0;
                    textLabel->SetText("");
                    lineDone = false;
                } else {
                    Engine::GameEngine::GetInstance().ChangeScene("personal");
                }
            }
        }
    }
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    for(auto bg: bgChange){
        int idx = bg.first;
        std::string path = bg.second;
        if(cur_line == idx && path != curBG){
            curBG = path;
            background = new Engine::Image(path, 0, 0, w, h);
            AddNewObject(background);
            break;
        } 
    }
}

void StoryScene::Draw() const {
    IScene::Draw();
    
    
    auto curBitmap = Engine::Resources::GetInstance().GetBitmap(curBG);
    if (curBitmap) {
        int w = al_get_bitmap_width(curBitmap.get());
        int h = al_get_bitmap_height(curBitmap.get());
        int screenW = Engine::GameEngine::GetInstance().GetScreenSize().x;
        int screenH = Engine::GameEngine::GetInstance().GetScreenSize().y;

        al_draw_scaled_bitmap(curBitmap.get(),
                            0, 0, w, h,
                            0, 0, screenW, screenH,
                            0);
    }


    ALLEGRO_COLOR white = al_map_rgb(255,255,255);
    ALLEGRO_COLOR black = al_map_rgb(0, 0, 0);
    float x = textLabel->Position.x;
    float y = textLabel->Position.y;
    float maxW = Engine::GameEngine::GetInstance().GetScreenSize().x - x - 50;
    auto font = Engine::Resources::GetInstance().GetFont("pirulen.ttf", 36).get();
    int lineH = al_get_font_line_height(font);

    std::string s = show_text;
    float padding = 20.0f;

    int row = 0;
    al_draw_filled_rectangle(x - padding, y - padding, x + 1480, y + 380 + padding, black);
    al_draw_rectangle(x - padding, y - padding, x + 1480, y + 380 + padding, white, 2.0f);
    while (!s.empty()) {
        int cut = s.size();
        while (cut > 0 && al_get_text_width(font, s.substr(0, cut).c_str()) > maxW)
            --cut;
        std::string part = s.substr(0, cut);
        al_draw_text(font, white, x, y + row * lineH, 0, part.c_str());
        s.erase(0, cut);
        ++row;
    }
    skipButton->Draw();
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
