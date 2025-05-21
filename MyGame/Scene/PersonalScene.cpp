//
// Created by Hsuan on 2024/4/10.
//

#include "StartScene.h"
#include <allegro5/allegro_audio.h>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <tuple>
#include <sstream>
#include <fstream>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "PlayScene.hpp"
#include "Scene/StartScene.h"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/Slider.hpp"

// TODO HACKATHON-2 (1/3): You can imitate the 2 files: 'StartScene.hpp', 'StartScene.cpp' to implement your SettingsScene.
void StartScene::Initialize() {
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;
    Engine::ImageButton *btn;

    AddNewObject(new Engine::Label("Tower Defense", "pirulen.ttf", 120, halfW, halfH / 3 + 50, 10, 255, 255, 255, 0.5, 0.5));
    //AddNewObject(new Engine::Label(, "pirulen.ttf", 120, halfW, halfH / 3 + 50, 10, 255, 255, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 200, halfH / 2 + 200, 400, 100);
    btn->SetOnClickCallback(std::bind(&StartScene::PlayOnClick, this, 1));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Play", "pirulen.ttf", 48, halfW, halfH / 2 + 250, 0, 0, 0, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("stage-select/dirt.png", "stage-select/floor.png", halfW - 200, halfH * 3 / 2 - 50, 400, 100);
    btn->SetOnClickCallback(std::bind(&StartScene::SettingsOnClick, this, 2));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Settings", "pirulen.ttf", 48, halfW, halfH * 3 / 2, 0, 0, 0, 255, 0.5, 0.5));

    Engine::GameEngine::UserProfile &user = Engine::GameEngine::GetInstance().GetCurrentUser();
    AddNewObject(new Engine::Label(user.name, "pirulen.ttf", 32, halfW, 300,255,255,255,255, 0.5,0.5));
    AddNewObject(new Engine::Label("Money: " + std::to_string(user.money), "pirulen.ttf", 32, halfW, 340,255,255,255,255, 0.5,0.5));
    AddNewObject(new Engine::Label("Level: " + std::to_string(user.level),"pirulen.ttf", 32, halfW, 370,255,255,255,255, 0.5,0.5));
}
void StartScene::Terminate() {
    const char* path = "C:\\Users\\annys\\Downloads\\2025_I2P2_TowerDefense-main\\2025_I2P2_TowerDefense-main\\Resource\\Log.txt";
    std::ifstream fin(path);
    std::unordered_map<std::string, std::tuple<std::string,int,int>> accounts;
    std::string line;
    while (std::getline(fin, line)) {
        std::istringstream iss(line);
        std::string u, p;
        int m, lv;
        if (iss >> u >> p >> m >> lv) {
            accounts[u] = std::make_tuple(p, m, lv);
        }
    }
    fin.close();

    auto &engine = Engine::GameEngine::GetInstance();
    auto up = engine.GetCurrentUser();  
    if (accounts.count(up.name)) {
        std::get<1>(accounts[up.name]) = up.money;
        std::get<2>(accounts[up.name]) = up.level;
    }

    std::ofstream fout(path, std::ios::trunc);
    for (auto &kv : accounts) {
        auto &name = kv.first;
        auto &[pass, money, level] = kv.second;
        fout << name << " " << pass  << " " << money << " " << level << "\n";
    }
    fout.close();

    IScene::Terminate();
}
void StartScene::PlayOnClick(int stage) {
    Engine::GameEngine::GetInstance().ChangeScene("stage-select");
}
void StartScene::SettingsOnClick(int stage) {
    Engine::GameEngine::GetInstance().ChangeScene("settings");
}