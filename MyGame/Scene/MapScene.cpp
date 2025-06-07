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
#include "Scene/MapScene.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/Slider.hpp"

void MapScene::Initialize() {
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;
    remainTime = 180.0f;
}
void MapScene::Update(float deltaTime) {

}
void MapScene::Terminate() {
    IScene::Terminate();
}