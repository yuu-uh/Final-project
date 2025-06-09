#include <functional>
#include <string>
#include <fstream>
#include <chrono>
#include <ctime>
#include <iomanip>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "Scene/PlayScene.hpp"
#include "UI/Component/Image.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "ResultScene.hpp"

// In your ResultScene.cpp, you can access the game results:

void ResultScene::Initialize() {
    // Access the game result data
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;
    auto& result = PlayScene::lastGameResult;
    
    std::string resultText;
    if (result.winnerId == 0) {
        resultText = "DRAW!";
    } else if (result.winnerId == NetWork::Instance().myId) {
        resultText = "YOU WIN!";
    } else {
        resultText = "YOU LOSE!";
    }
    
    // Create result labels
    Engine::Label* resultLabel = new Engine::Label(
        resultText, "pirulen.ttf", 48, 
        halfW, halfH - 100, 
        255, 255, 255, 255, 0.5, 0.5
    );
    
    // Show final lives
    std::string livesText = "Final Lives - You: " + 
        std::to_string((NetWork::Instance().myId == 1) ? result.player1Lives : result.player2Lives) +
        " Opponent: " + 
        std::to_string((NetWork::Instance().myId == 1) ? result.player2Lives : result.player1Lives);
        
    Engine::Label* livesLabel = new Engine::Label(
        livesText, "pirulen.ttf", 24,
        halfW, halfH, 
        255, 255, 255, 255, 0.5, 0.5
    );
    
    // Show end reason
    std::string reasonText = (result.endReason == 0) ? "Game ended: Lives depleted" : "Game ended: Time up";
    Engine::Label* reasonLabel = new Engine::Label(
        reasonText, "pirulen.ttf", 20,
        halfW, halfH + 50,
        200, 200, 200, 255, 0.5, 0.5
    );
    
    UIGroup->AddNewObject(resultLabel);
    UIGroup->AddNewObject(livesLabel);
    UIGroup->AddNewObject(reasonLabel);
}

void ResultScene::Terminate() {
    IScene::Terminate();
    AudioHelper::StopBGM(bgmId);
}
void ResultScene::Update(float deltaTime) {
    ticks += deltaTime;
    // if (ticks > 4 && ticks < 100 &&
    //     dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetScene("play"))->MapId == 2) {
    //     ticks = 100;
    //     //bgmId = AudioHelper::PlayBGM("happy.ogg");
    // }
}
void ResultScene::BackOnClick(int stage) {
    Engine::GameEngine::GetInstance().ChangeScene("personal");
}