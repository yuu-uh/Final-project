#include "LogScene.hpp"
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_image.h> 
#include <functional>
#include <fstream>
#include <memory>
#include <string>
#include <sstream>
#include <unordered_map>
#include <tuple>   

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "Engine/Group.hpp"
#include "Scene/PersonalScene.hpp"
#include "Scene/LogScene.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/Slider.hpp"

void LogScene::Initialize() {
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;

    AddNewObject(new Engine::Image("background/test.png" ,0, 0, w, h));

    Engine::ImageButton *btn;
    AddNewObject(new Engine::Label("Log in to start", "pirulen.ttf", 100, halfW, halfH / 3 + 50, 255, 255, 255, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("stage-select/floor.png", "stage-select/floor.png", halfW, halfH/2+200, 400, 100);
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Name", "pirulen.ttf", 48, halfW-310, halfH/2+250, 255, 255, 255, 255, 0.5, 0.5));
    name.clear();
    nameLabel = new Engine::Label("", "pirulen.ttf", 32, halfW+200, halfH/2+250, 0, 0, 0, 255, 0.5, 0.5);
    AddNewObject(nameLabel);

    btn = new Engine::ImageButton("stage-select/floor.png", "stage-select/floor.png", halfW, halfH * 3 / 2 - 50, 400, 100);
    //btn->SetOnClickCallback(std::bind(&LogScene::SettingsOnClick, this, 2));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Password", "pirulen.ttf", 48, halfW-400, halfH * 3 / 2, 255, 255, 255, 255, 0.5, 0.5));
    password.clear();
    passwordLabel = new Engine::Label("", "pirulen.ttf", 32, halfW+200, halfH * 3 / 2, 0, 0, 0, 255, 0.5, 0.5);
    AddNewObject(passwordLabel);
}
void LogScene::Terminate() {
    IScene::Terminate();
}

void LogScene::OnKeyDown(int keyCode) {
    IScene::OnKeyDown(keyCode);

    std::string &target      = editingName ? name : password;
    Engine::Label* targetLbl = editingName ? nameLabel : passwordLabel;

    if (keyCode == ALLEGRO_KEY_BACKSPACE) {
        if (!target.empty()) target.pop_back();
    }
    else if (ALLEGRO_KEY_A <= keyCode && keyCode <= ALLEGRO_KEY_Z) {
        target.push_back('A' + (keyCode - ALLEGRO_KEY_A));
    }
    else if (ALLEGRO_KEY_0 <= keyCode && keyCode <= ALLEGRO_KEY_9) {
        target.push_back('0' + (keyCode - ALLEGRO_KEY_0));
    }
    else if (keyCode == ALLEGRO_KEY_SPACE) {
        target.push_back(' ');
    }
    else if (keyCode == ALLEGRO_KEY_ENTER) {
        if (editingName) editingName = false;  
        else { 
            std::ifstream fin("C:\\Users\\annys\\Downloads\\FinalProject\\MyGame\\Resource\\Log.txt");
            std::unordered_map<std::string, std::tuple<std::string,int,int>> accounts;
            std::string line;
            while (std::getline(fin, line)) {
                std::istringstream iss(line);
                std::string user, pass;
                int money, level;
                if (iss >> user >> pass >> money >> level) accounts[user] = std::make_tuple(pass, money, level);
            }
            fin.close();

            std::ofstream fout("C:\\Users\\annys\\Downloads\\FinalProject\\MyGame\\Resource\\Log.txt", std::ios::app);
            auto it = accounts.find(name);
            Engine::GameEngine::UserProfile up;
            if (it == accounts.end()){
                up.name  = name;
                up.money = 0;
                up.level = 1;
                fout << name << " " << password << " 0 1" << '\n';
            } 
            else {
                auto& [realPass, money, level] = it->second;
                if (realPass != password) {
                    passwordLabel->Text = "Wrong";
                    return;
                }
                up.name  = name;
                up.money = money;
                up.level = level;
            }

            Engine::GameEngine::GetInstance().SetCurrentUser(up);
            Engine::GameEngine::GetInstance().ChangeScene("personal");
            return;  
        }
    }
    targetLbl->Text = target;
}