#include <functional>
#include <string>
#include <fstream>
#include <chrono>
#include <ctime>
#include <iomanip>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "PlayScene.hpp"
#include "UI/Component/Image.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "WinScene.hpp"

void WinScene::Initialize() {
    ticks = 0;
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;
    AddNewObject(new Engine::Image("win/benjamin-sad.png", halfW, halfH, 0, 0, 0.5, 0.5));
    AddNewObject(new Engine::Label("You Win!", "pirulen.ttf", 48, halfW, halfH / 4 - 10, 255, 255, 255, 255, 0.5, 0.5));
    
    Engine::ImageButton *btn;
    btn = new Engine::ImageButton("win/dirt.png", "win/floor.png", halfW - 200, halfH * 7 / 4 - 50, 400, 100);
    btn->SetOnClickCallback(std::bind(&WinScene::BackOnClick, this, 2));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Back", "pirulen.ttf", 48, halfW, halfH * 7 / 4, 0, 0, 0, 255, 0.5, 0.5));
    bgmId = AudioHelper::PlayAudio("win.wav");

    btn = new Engine::ImageButton("win/dirt.png", "win/floor.png", halfW - 400, 200, 800, 200);
    //btn->SetOnClickCallback(std::bind(&WinScene::BackOnClick, this, 2));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("enter your name", "pirulen.ttf", 48, halfW, 250, 0, 0, 0, 255, 0.5, 0.5));
    playerName.clear();
    nameLabel = new Engine::Label("", "pirulen.ttf", 32, halfW, 320, 0, 0, 0, 255, 0.5, 0.5);
    AddNewObject(nameLabel);

    Engine::GameEngine::UserProfile &user = Engine::GameEngine::GetInstance().GetCurrentUser();
    user.money += score;
    while(user.money >= 10000){
        user.money -= 10000;
        user.level++;
    }
}
void WinScene::Terminate() {
    IScene::Terminate();
    AudioHelper::StopBGM(bgmId);
}
void WinScene::Update(float deltaTime) {
    ticks += deltaTime;
    if (ticks > 4 && ticks < 100 &&
        dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetScene("play"))->MapId == 2) {
        ticks = 100;
        bgmId = AudioHelper::PlayBGM("happy.ogg");
    }
}
void WinScene::BackOnClick(int stage) {
    // Change to select scene.
    Engine::GameEngine::GetInstance().ChangeScene("stage-select");
}
void WinScene::OnKeyDown(int keyCode) {
    IScene::OnKeyDown(keyCode);
    if (keyCode == ALLEGRO_KEY_BACKSPACE) {
        if (!playerName.empty()) playerName.pop_back();
    }
    else if (ALLEGRO_KEY_A <= keyCode && keyCode <= ALLEGRO_KEY_Z) {
        char c = 'A' + (keyCode - ALLEGRO_KEY_A);
        playerName.push_back(c);
    }
    else if (ALLEGRO_KEY_0 <= keyCode && keyCode <= ALLEGRO_KEY_9) {
        char c = '0' + (keyCode - ALLEGRO_KEY_0);
        playerName.push_back(c);
    }
    else if (keyCode == ALLEGRO_KEY_SPACE) playerName.push_back(' ');
    else if (keyCode == ALLEGRO_KEY_ENTER) {
        auto now = std::chrono::system_clock::now();
        std::time_t t = std::chrono::system_clock::to_time_t(now);

        std::ofstream fout("C:\\Users\\annys\\Downloads\\2025_I2P2_TowerDefense-main\\2025_I2P2_TowerDefense-main\\Resource\\scoreboard1.txt", std::ios::app);
        if (fout) {
            fout << playerName << " "
                << score  << " "
                << std::put_time(std::localtime(&t), "%m/%d %H:%M")
                << "\n";
        }
        Engine::GameEngine::GetInstance().ChangeScene("stage-select");
    }
    nameLabel->Text = playerName;
}