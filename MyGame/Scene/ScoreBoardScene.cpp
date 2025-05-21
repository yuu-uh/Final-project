#include <allegro5/allegro_audio.h>
#include <functional>
#include <fstream>
#include <sstream>
#include <memory>
#include <string>
#include <vector> 
#include <algorithm> 

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "PlayScene.hpp"
#include "Scene/ScoreBoardScene.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/Slider.hpp"

struct Entry {
    std::string name;       
    int score;      
    std::string timestamp;  
};

void ScoreBoardScene1::Initialize() {
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;
    Engine::ImageButton *btn;
    
    AddNewObject(new Engine::Label("Score board 1", "pirulen.ttf", 48, halfW, 50, 10, 255, 255, 255, 0.5, 0.5));

    std::ifstream fin("C:\\Users\\annys\\Downloads\\2025_I2P2_TowerDefense-main\\2025_I2P2_TowerDefense-main\\Resource\\scoreboard1.txt");
    std::vector<Entry> data;
    if (fin.is_open()) {
        std::string line;
        while (std::getline(fin, line)) {
            std::istringstream iss(line);
            Entry e;
            std::string date, time;
            iss >> e.name >> e.score >> date >> time;
            e.timestamp = date + " " + time;
            data.push_back(e);
        }
        fin.close();
    }
    std::sort(data.begin(), data.end(), [](auto &a, auto &b){ return a.score > b.score; });
    int y = 120;
    for (auto &e : data) {
        std::string text = e.name + " "
                         + std::to_string(e.score) + " "
                         + e.timestamp;
        AddNewObject(new Engine::Label(
          text, "pirulen.ttf", 24, halfW, y,
          255, 255, 255, 255, 0.5, 0.5
        ));
        y += 32;
    }
    
    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 200, halfH * 3 / 2 - 50, 400, 100);
    btn->SetOnClickCallback(std::bind(&ScoreBoardScene1::BackOnClick, this, 1));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Back", "pirulen.ttf", 48, halfW, halfH * 3 / 2, 0, 0, 0, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW + 300, halfH * 3 / 2 - 50, 400, 100);
    btn->SetOnClickCallback(std::bind(&ScoreBoardScene1::NextOnClick, this));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Next page", "pirulen.ttf", 48, halfW + 500, halfH * 3 / 2, 0, 0, 0, 255, 0.5, 0.5));
}
void ScoreBoardScene1::Terminate() {
    IScene::Terminate();
}
void ScoreBoardScene1::BackOnClick(int stage) {
    Engine::GameEngine::GetInstance().ChangeScene("stage-select");
}
void ScoreBoardScene1::NextOnClick() {
    Engine::GameEngine::GetInstance().ChangeScene("scoreboard2");
}