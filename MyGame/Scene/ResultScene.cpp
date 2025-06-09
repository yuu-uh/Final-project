#include <functional>
#include <string>
#include <fstream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <tuple>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "Scene/PlayScene.hpp"
#include "UI/Component/Image.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "ResultScene.hpp"
#include "Engine/NetWork.hpp"
#include "Engine/LOG.hpp"

// In your ResultScene.cpp, you can access the game results:

void ResultScene::Initialize() {
    if (!NetWork::Instance().isConnected()) return;

    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;

    auto& result = PlayScene::lastGameResult;
    Engine::LOG(Engine::INFO) << "initialize result scene";

    int myId = NetWork::Instance().myId;
    int opponentId = 3 - myId;

    int winner = result.winnerId;
    int loser = (winner == 1) ? 2 : 1;

    // === PLAYER INFO POSITIONS ===
    int leftX = w / 4;
    int rightX = 3 * w / 4;
    int textY = halfH - 150;
    int imageY = textY + 80;

    // auto createPlayerDisplay = [&](int playerId, int x, bool isWinner) {
    //     auto p1 = PlayScene::player1;
    //     auto p2 = PlayScene::player2;


    //     std::string text = isWinner ? "WINNER" : "LOSER";
    //     Engine::Label* label = new Engine::Label(
    //         text, "pirulen.ttf", 36,
    //         x, textY,
    //         isWinner ? 255 : 180, isWinner ? 255 : 80, isWinner ? 0 : 0, 255,
    //         0.5, 0.5
    //     );
    //     AddNewObject(label);

    //     ALLEGRO_BITMAP* bmp = (playerId == 1) ? p1->animations.at("front").at(0)
    //                                         : p2->animations.at("front").at(0);

    //     renderPlayers.push_back(PlayerRenderInfo{
    //         bmp,
    //         x,
    //         imageY,
    //         isWinner,
    //         "Lives: " + std::to_string((playerId == 1) ? result.player1Lives : result.player2Lives)
    //     });

    //     Engine::Label* livesLabel = new Engine::Label(
    //         renderPlayers.back().livesText, "pirulen.ttf", 20,
    //         x, imageY + 140,
    //         255, 255, 255, 255, 0.5, 0.5
    //     );
    //     AddNewObject(livesLabel);
    // };

    // Left: Loser
    // if (winner == 0) {
    //     // Draw case, show both
    //     createPlayerDisplay(1, leftX, false);
    //     createPlayerDisplay(2, rightX, false);
    // } else {
    //     createPlayerDisplay(loser, leftX, false);
    //     createPlayerDisplay(winner, rightX, true);
    // }

    // Center result reason
    std::string reasonText = (result.endReason == 0) ? "Game ended: Lives depleted" : "Game ended: Time up";
    Engine::Label* reasonLabel = new Engine::Label(
        reasonText, "pirulen.ttf", 20,
        halfW, halfH + 100,
        200, 200, 200, 255, 0.5, 0.5
    );
    AddNewObject(reasonLabel);

    // === CENTERED BACK BUTTON ===
    Engine::ImageButton* btn = new Engine::ImageButton(
        "GUI/button2.png", "GUI/button2.png",
        halfW - 135, h - 180, 270, 70
    );
    btn->SetOnClickCallback(std::bind(&ResultScene::BackOnClick, this, 1));
    AddNewControlObject(btn);
    AddNewObject(new Engine::Label("Back", "pirulen.ttf", 36,
        halfW, h - 145, 255, 255, 255, 255, 0.5, 0.5));

    Engine::LOG(Engine::INFO) << "finished initialize result scene";
}

void ResultScene::Terminate() {
    IScene::Terminate();
    //AudioHelper::StopBGM(bgmId);
}
void ResultScene::Update(float deltaTime) {
    ticks += deltaTime;
    // if (ticks > 4 && ticks < 100 &&
    //     dynamic_cast<PlayScene *>(Engine::GameEngine::GetInstance().GetScene("play"))->MapId == 2) {
    //     ticks = 100;
    //     //bgmId = AudioHelper::PlayBGM("happy.ogg");
    // }
    IScene::Update(deltaTime);
}
void ResultScene::BackOnClick(int stage) {
    Engine::GameEngine::GetInstance().ChangeScene("personal");
}