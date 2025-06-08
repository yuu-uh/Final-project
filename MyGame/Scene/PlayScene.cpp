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
#include "UI/Component/Label.hpp"
#include "UI/Component/ImageButton.hpp"
#include "Scene/MapScene.hpp"
#include "Soldier/Soldier.hpp"
#include "Soldier/ninja.hpp"
#include "Soldier/master.hpp"
#include "Soldier/dragen.hpp"
#include "Soldier/shooter.hpp"
#include "Soldier/slime.hpp"
#include "Soldier/vikin.hpp"

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

    // int panelX0 = 1300, panelY0 = 320;
    // const int pad     = 8;
    // const int iconW   = 128, iconH = 128;
    // const int cols    = 2;
    // for (int idx = 0; idx < 6; ++idx) {
    //     int col = idx % cols;
    //     int row = idx / cols;
    //     float x = panelX0 + pad + col * (iconW + pad);
    //     float y = panelY0 + pad + row * (iconH + pad);
    //     UIInventoryGroup->AddNewObject(
    //         new Engine::Image("mapScene/item_empty.png", x, y, iconW, iconH)
    //     );
    // }

    // auto &picked = Engine::GameEngine::GetInstance().pickedItems;
    // for (size_t i = 0; i < picked.size() && i < 6; ++i) {
    //     int col = i % cols;
    //     int row = i / cols;
    //     float x = panelX0 + pad + col * (iconW + pad);
    //     float y = panelY0 + pad + row * (iconH + pad);

    //     std::string path = "mapScene/" + picked[i] + ".png";
    //     UIInventoryGroup->AddNewObject(
    //         new Engine::Image(path, x, y, 100, 100)
    //     );
    // }

    ConstructUI();

    timer = 300.0f;
    countdownLabel = new Engine::Label("05:00", "pirulen.ttf", 48,
    Engine::GameEngine::GetInstance().GetScreenSize().x - 120,
    Engine::GameEngine::GetInstance().GetScreenSize().y - 40, 255, 255, 255, 255, 0.5, 0.5);
    UIGroup->AddNewObject(countdownLabel);

    livelabel = new Engine::Label(
        std::string("Life ") + std::to_string(lives),
        "pirulen.ttf", 48, 1400, 50, 255,255,255,255, 0.5,0.5);
    UIGroup->AddNewObject(livelabel);
    
    imgTarget = new Engine::Image("play/target.png", 0, 0);
    imgTarget->Visible = false;
    UIGroup->AddNewObject(imgTarget);
    preview = nullptr;
    //SoldierGroup->AddNewObject(new Master(300, 300));

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
    if (preview) {
        preview->Position = Engine::GameEngine::GetInstance().GetMousePosition();
        // To keep responding when paused.
        preview->Update(deltaTime);
    }
    SoldierGroup->Update(deltaTime);
    UIGroup->Update(deltaTime);
    UIInventoryGroup->Update(deltaTime);
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
    UIGroup->Draw();
    UIInventoryGroup->Draw();
    SoldierGroup->Draw();
    if (MouseOnIcon) MouseOnIcon->Draw();  
}
void PlayScene::OnMouseDown(int button, int mx, int my) {
    if ((button & 1) && !imgTarget->Visible && preview) {
        UIGroup->RemoveObject(preview->GetObjectIterator());
        preview = nullptr;
    }else if(preview){
        OnMouseMove(mx, my);
    }
    IScene::OnMouseDown(button, mx, my);
}
void PlayScene::OnMouseMove(int mx, int my) {
    IScene::OnMouseMove(mx, my);
    const int x = mx / BlockSize;
    const int y = my / BlockSize;
    if (!preview || x < 0 || x >= MapWidth || y < 0 || y >= MapHeight) {
        imgTarget->Visible = false;
        return;
    }
    imgTarget->Visible = true;
    imgTarget->Position.x = x * BlockSize;
    imgTarget->Position.y = y * BlockSize;
}

void PlayScene::OnMouseUp(int button, int mx, int my) {
    IScene::OnMouseUp(button, mx, my);
    if (!imgTarget->Visible)
        return;
    const int x = mx / BlockSize;
    const int y = my / BlockSize;
    if (button & 1) {
        //if (mapState[y][x] != TILE_OCCUPIED) {
            if (!preview)
                return;
            // Check if valid.
            bool valid = false;
            valid = CheckSpaceValid(x, y);
            
            if (!valid) {
                Engine::Sprite *sprite;
                //GroundEffectGroup->AddNewObject(sprite = new DirtyEffect("play/target-invalid.png", 1, x * BlockSize + BlockSize / 2, y * BlockSize + BlockSize / 2));
                sprite->Rotation = 0;
                return;
            }
            // Purchase.
            // Remove Preview.
            preview->GetObjectIterator()->first = false;
            UIGroup->RemoveObject(preview->GetObjectIterator());
            // Construct real turret.
            preview->Position.x = x * BlockSize + BlockSize / 2;
            preview->Position.y = y * BlockSize + BlockSize / 2;
            preview->Enabled = true;
            preview->Preview = false;
            preview->Tint = al_map_rgba(255, 255, 255, 255);
            SoldierGroup->AddNewObject(preview);
            // To keep responding when paused.
            preview->Update(0);
            // Remove Preview.
            preview = nullptr;
            //mapState[y][x] = TILE_OCCUPIED;
            OnMouseMove(mx, my);
        //}
    }
}

void PlayScene::Hit() {
    lives--;
    UILives->Text = std::string("Life ") + std::to_string(lives);
    if (lives <= 0) {
        Engine::GameEngine::GetInstance().ChangeScene("result");
    }
}

void PlayScene::ConstructUI() {
    int panelX0 = 1300, panelY0 = 320;
    const int pad     = 30;
    const int iconW   = 100, iconH = 100;
    const int cols    = 320 / (iconW + pad);
    for (int idx = 0; idx < 6; ++idx) {
        int col = idx % cols;
        int row = idx / cols;
        float x = panelX0 + pad + col * (iconW + pad);
        float y = panelY0 + pad + row * (iconH + pad);
        UIInventoryGroup->AddNewObject(
            new Engine::Image("mapScene/item_empty.png", x, y, iconW, iconH)
        );
    }

    auto &picked = Engine::GameEngine::GetInstance().pickedItems;
    for (size_t i = 0; i < picked.size() && i < 6; ++i) {
        int col = i % cols;
        int row = i / cols;
        float x = panelX0 + pad + col * (iconW + pad);
        float y = panelY0 + pad + row * (iconH + pad);

        std::string path = "mapScene/" + picked[i] + ".png";
        UIInventoryGroup->AddNewObject(
            new Engine::Image(path, x, y, 100, 100)
        );
    }

    Engine::ImageButton* btn;
    for (size_t i = 0; i < picked.size() && i < 6; ++i) {
        int col = i % cols;
        int row = i / cols;
        float x = panelX0 + pad + col * (iconW + pad);
        float y = panelY0 + pad + row * (iconH + pad);

        std::string path = "mapScene/" + picked[i] + ".png";
        btn = new Engine::ImageButton(path, path, x, y, 100, 100);
        btn->SetOnClickCallback(std::bind(&PlayScene::UIBtnClicked, this, picked[i]));
        UIGroup->AddNewControlObject(btn);
    }

    auto& itemCountMap = Engine::GameEngine::GetInstance().itemCount;
    for (size_t i = 0; i < picked.size() && i < 6; ++i) {
        int col = i % cols;
        int row = i / cols;
        float x = panelX0 + pad + col * (iconW + pad);
        float y = panelY0 + pad + row * (iconH + pad);
        std::string itemType = picked[i];
        if (itemCountMap.count(itemType)) {
            int cnt = itemCountMap[itemType].first;
            std::string text = "x" + std::to_string(itemCountMap[itemType].first);
            Engine::Label* label = new Engine::Label(
                text, "pirulen.ttf", 24,
                x + 100 - 10, y + 100 - 10, 255, 255, 255, 255, 1.0, 1.0);
            UIInventoryGroup->AddNewObject(label);
            // If you want to track it for updating later:
            LocalItemCount[itemType] = std::make_pair(cnt, label);
        }
    }

}

void PlayScene::UIBtnClicked(std::string type) {
    Soldier* next_preview = nullptr;
    if(LocalItemCount[type].first <= 0)
        return;

    if(type == "ninja"){
        next_preview = new Ninja(0, 0);
    }else if(type == "vikin"){
        next_preview = new Vikin(0, 0);
    }else if(type == "master"){
        next_preview = new Master(0, 0);
    }else if(type == "shooter"){
        next_preview = new Shooter(0, 0);
    }else if(type == "slime"){
        next_preview = new Slime(0, 0);
    }else if(type == "dragen"){
        next_preview  = new Dragen(0, 0);
    }
    if(!next_preview) return;

    if(preview)
        UIGroup->RemoveObject(preview->GetObjectIterator());
    preview = next_preview;
    preview->Position = Engine::GameEngine::GetInstance().GetMousePosition();
    preview->Tint = al_map_rgba(255, 255, 255, 200);
    preview->Enabled = false;
    preview->Preview = true;
    UIGroup->AddNewObject(preview);
    OnMouseMove(Engine::GameEngine::GetInstance().GetMousePosition().x, Engine::GameEngine::GetInstance().GetMousePosition().y); 
}

bool PlayScene::CheckSpaceValid(int x, int y) {
    if (x < 0 || x >= MapWidth || y < 0 || y >= MapHeight)
        return false;
    return true;
}
