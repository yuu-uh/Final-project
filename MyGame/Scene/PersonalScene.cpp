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
#include "Scene/PersonalScene.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/Slider.hpp"

void PersonalScene::Initialize() {
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;
    Engine::ImageButton *btn;

    AddNewObject(new Engine::Image("background/test.png" ,0, 0, w, h));
    AddNewObject(new Engine::Label("Hello", "pirulen.ttf", 100, halfW, halfH/3+50, 255, 255, 255, 255, 0.5, 0.5));
    
    btn = new Engine::ImageButton("stage-select/floor.png", "stage-select/floor.png", halfW-200, halfH/2+100, 400, 100);
    AddNewControlObject(btn);
    btn->SetOnClickCallback(std::bind(&PersonalScene::MapOnClick, this));
    AddNewObject(new Engine::Label("Play", "pirulen.ttf", 48, halfW, halfH/2+150, 255, 255, 255, 255, 0.5, 0.5));

    btn = new Engine::ImageButton("stage-select/floor.png", "stage-select/floor.png", halfW-200, halfH/2+300, 400, 100);
    AddNewControlObject(btn);
    btn->SetOnClickCallback(std::bind(&PersonalScene::ScoreOnClick, this));
    AddNewObject(new Engine::Label("Score", "pirulen.ttf", 48, halfW, halfH/2+350, 255, 255, 255, 255, 0.5, 0.5));
}
void PersonalScene::Terminate() {
    IScene::Terminate();
}
void PersonalScene::MapOnClick() {
    Engine::GameEngine::GetInstance().ChangeScene("play");
}
void PersonalScene::ScoreOnClick() {
    Engine::GameEngine::GetInstance().ChangeScene("score");
}