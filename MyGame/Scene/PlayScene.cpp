#include <algorithm>
#include <allegro5/allegro.h>
#include <cmath>
#include <fstream>
#include <functional>
#include <memory>
#include <queue>
#include <string>
#include <vector>
#include <sstream>

#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/IScene.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "Engine/LOG.hpp"
#include "Engine/Collider.hpp"
#include "Engine/Group.hpp"
#include "PlayScene.hpp"
#include "Items/Item.hpp"
#include "UI/Animation/DirtyEffect.hpp"
#include "UI/Component/Label.hpp"
#include "Soldier/Soldier.hpp"

const std::vector<Engine::Point> PlayScene::directions = { Engine::Point(-1, 0), Engine::Point(0, -1), Engine::Point(1, 0), Engine::Point(0, 1) };
const int PlayScene::MapWidth = 20, PlayScene::MapHeight = 13;
const int PlayScene::BlockSize = 64;
Engine::Point PlayScene::GetClientSize() {
    return Engine::Point(MapWidth * BlockSize, MapHeight * BlockSize);
}
const std::vector<std::string> PlayScene::itemImg = {"ninja", "master", "slime", "vikin", "dragen", "shooter", "magician"};

void PlayScene::Initialize() {
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;
    AddNewObject(TileMapGroup = new Group());
    AddNewObject(GroundEffectGroup = new Group());
    AddNewObject(ItemGroup = new Group);
    AddNewObject(SoldierGroup = new Group());
    AddNewControlObject(UIGroup = new Group());
    AddNewControlObject(UIInventoryGroup = new Group());
    ReadMap();

    timer = 300.0f;
    countdownLabel = new Engine::Label("05:00", "pirulen.ttf", 48,
    Engine::GameEngine::GetInstance().GetScreenSize().x - 120,
    Engine::GameEngine::GetInstance().GetScreenSize().y - 40, 255, 255, 255, 255, 0.5, 0.5);
    UIGroup->AddNewObject(countdownLabel);

    livelabel = new Engine::Label(
        std::string("Life ") + std::to_string(lives),
        "pirulen.ttf", 48, 1400, 50, 255,255,255,255, 0.5,0.5);
    UIGroup->AddNewObject(livelabel);

    ConstructUI();
}
void PlayScene::Terminate() {
    AudioHelper::StopBGM(bgmId);
    AudioHelper::StopSample(deathBGMInstance);
    deathBGMInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    IScene::Terminate();
}
void PlayScene::Update(float deltaTime) {
    IScene::Update(deltaTime);
    timer -= deltaTime;
    if (timer <= 0) {
        Engine::GameEngine::GetInstance().ChangeScene("log");
        return;
    }
    int minutes = int(timer) / 60;
    int seconds = int(timer) % 60;
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << minutes
        << ":" << std::setw(2) << std::setfill('0') << seconds;
    countdownLabel->Text = oss.str();
    SoldierGroup->Update(deltaTime);
}
void PlayScene::ReadMap() {
    std::ifstream fin("Resource/Play.txt");
    mapData.assign(MapHeight, std::vector<int>(MapWidth, 0));
    std::string line;
    for (int y = 0; y < MapHeight; y++) {
        if (!std::getline(fin, line) || (int)line.size() < MapWidth)
            throw std::runtime_error("Map.txt 行长不够");
        for (int x = 0; x < MapWidth; x++)
            mapData[y][x] = line[x] - '0';
    }
}
void PlayScene::Draw() const {
    IScene::Draw();
    for (int y = 0; y < MapHeight; y++) {
        for (int x = 0; x < MapWidth; x++) {
            std::cout<<mapData[y][x];
        }
        std::cout<<std::endl;
    }
    for (int y = 0; y < MapHeight; y++) {
        for (int x = 0; x < MapWidth; x++) {
            // const char* path = mapData[y][x]
            //     ? "mapScene/stone.png"
            //     : "mapScene/grass.png";
            const char* path = nullptr;
            switch (mapData[y][x]){
            case 0:
                path = "mapScene/grass.png";
                break;
            case 1:
                path = "mapScene/stone.png";
                break;
            case 2:
                path = "mapScene/brick.png";
                break;
            case 3:
                path = "mapScene/roof.png";
                break;
            default:
                break;
            }
            if (!path) {
                Engine::LOG(Engine::ERROR) << "Invalid map value: " << mapData[y][x];
                continue; // Skip drawing this tile
            }
            ALLEGRO_BITMAP* bmp = Engine::Resources::GetInstance()
                                      .GetBitmap(path).get();
            if(!bmp){
                Engine::LOG(Engine::INFO)<<"failed to load map bitmap";
            }
            al_draw_scaled_bitmap(
                bmp, 0, 0, al_get_bitmap_width(bmp), al_get_bitmap_height(bmp),
                x * BlockSize, y * BlockSize, BlockSize, BlockSize,0
            );
        }
    }
    for (auto& it : worldItems) it->Draw();
    if (MouseOnIcon) MouseOnIcon->Draw();
    UIGroup->Draw();
    UIInventoryGroup->Draw();
    //SoldierGroup->Draw();
}
void PlayScene::OnMouseDown(int button, int mx, int my) {
    if (!(button & 1)) return;

    float sw = Engine::GameEngine::GetInstance().GetScreenSize().x;
    if (mx > sw - 320) {
        int localX = mx - int(sw - 320);
        int localY = my;
        const int pad = 8, w = 48, h = 48, cols = 2;
        int col = localX / (w + pad);
        int row = localY / (h + pad);
        int idx = row * cols + col;
        if (idx >= 0 && idx < (int)inventoryImgs.size()) {
            MouseIdx = idx;
            MouseOnIcon = new Engine::Image(
                "mapScene/" + inventoryImgs[idx] + ".png",
                mx - w/2, my - h/2,
                w, h
            );
        }
    }
}
void PlayScene::OnMouseMove(int mx, int my) {
   if (MouseOnIcon) {
        MouseOnIcon->Position.x = mx - 24;
        MouseOnIcon->Position.y = my - 24;
    }
}
void PlayScene::OnMouseUp(int button, int mx, int my) {
    if (!(button & 1) || !MouseOnIcon) return;

    int gx = mx / BlockSize;
    int gy = my / BlockSize;
    bool canPlace = gx >= 0 && gx < MapWidth
                 && gy >= 0 && gy < MapHeight
                 && (mapData[gy][gx] == 1 || mapData[gy][gx] == 0);

    if (canPlace) {
        auto it = std::make_shared<Item>(
            inventoryImgs[MouseIdx] + ".png",
            gx * BlockSize,
            gy * BlockSize,
            inventoryImgs[MouseIdx]
        );
        worldItems.push_back(it);
    }

    delete MouseOnIcon;
    MouseOnIcon = nullptr;
    MouseIdx = -1;
}
void PlayScene::Hit() {
    lives--;
    UILives->Text = std::string("Life ") + std::to_string(lives);
    if (lives <= 0) {
        Engine::GameEngine::GetInstance().ChangeScene("result");
    }
}
void PlayScene::ConstructUI() {
    const int panelX0 = 1300;
    const int panelY0 = 320;
    const int pad     = 8;
    const int iconW   = 128, iconH = 128;
    const int cols    = (320) / (iconW + pad);

    for (int idx = 0; idx < 6; idx++) {
        int col = idx % cols;
        int row = idx / cols;
        float x = panelX0 + pad + col * (iconW + pad);
        float y = panelY0 + pad + row * (iconH + pad);
        auto empty = new Engine::Image("mapScene/item_picked.png", x, y,iconW, iconH);
        UIInventoryGroup->AddNewObject(empty);
    }
}

void PlayScene::UIBtnClicked(int id) {
    
}

bool PlayScene::CheckSpaceValid(int x, int y) {
    if (x < 0 || x >= MapWidth || y < 0 || y >= MapHeight)
        return false;
    return true;
}
