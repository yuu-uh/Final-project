#include <functional>
#include <string>
#include <fstream>
#include <sstream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <tuple>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/Point.hpp"
#include "Scene/PlayScene.hpp"
#include "ResultScene.hpp"
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

    AddNewObject(new Engine::Image("background/battleback8.png" ,0, 0, w, h));


    auto createPlayerDisplay = [&](int playerId, int x, bool isWinner) {
        std::string str = "button";
        std::string path = "GUI/"+str+".png";
        Engine::Image* img = new Engine::Image(path, x-200, textY-50, 400, 100);
        AddNewObject(img);

        std::string text = isWinner ? "WINNER" : "LOSER";
        Engine::Label* label = new Engine::Label(
            text, "pirulen.ttf", 48,
            x, textY,
            255, 255, 255, 255,
            0.5, 0.5
        );
        AddNewObject(label);

        // Get image path instead of bitmap
        std::string imgPath = (playerId == 1) ? "mapScene/Engineer_front01.png" : "mapScene/Engineer_front01.png";

        Engine::Image* playerImage = new Engine::Image(
            imgPath,
            x, imageY,
            128, 128  // width and height (adjust if needed)
        );
        AddNewObject(playerImage);

        // Update renderPlayers if needed to keep track of images
        renderPlayers.push_back(PlayerRenderInfo{
            nullptr, // no bitmap now
            x,
            imageY,
            isWinner,
            "Lives: " + std::to_string((playerId == 1) ? result.player1Lives : result.player2Lives)
        });

        Engine::Label* livesLabel = new Engine::Label(
            renderPlayers.back().livesText, "pirulen.ttf", 48,
            x, imageY + 140,
            255, 255, 255, 255, 0.5, 0.5
        );
        AddNewObject(livesLabel);
    };

    
    if (winner == 0) {
        // Draw case, show both
        createPlayerDisplay(1, leftX, false);
        createPlayerDisplay(2, rightX, false);
    } else {
        createPlayerDisplay(loser, leftX, false);
        createPlayerDisplay(winner, rightX, true);
    }

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
    SaveResult();
    Engine::LOG(Engine::INFO) << "finished initialize result scene";
    bgmId = AudioHelper::PlayBGM("others.ogg");
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

std::string getDateTime(){
    std::time_t now = std::time(nullptr);
    std::tm* t = std::localtime(&now);

    std::stringstream oss;
    oss << std::put_time(t, "%Y/%m/%d");
    return oss.str();
}

void ResultScene::SaveResult(){
    int myId = NetWork::Instance().myId;
    auto& result = PlayScene::lastGameResult;

    bool isWinner = (result.winnerId == myId);
    int livesLeft = (myId == 1) ? result.player1Lives : result.player2Lives;

    std::ofstream ofs("Resource/ScoreBoard.txt", std::ios::app); // append mode
    if (!ofs) {
        Engine::LOG(Engine::ERROR) << "Failed to open player_results.txt for writing";
        return;
    }
    ofs << (isWinner ? "Win" : "Lose") << " " << livesLeft<< " " << getDateTime() <<"\n";
    ofs.close();
}