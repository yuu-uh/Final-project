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
#include "Engine/Message.hpp"


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

    NetWork::Instance().SetReceiveCallback(
        std::bind(&PlayScene::HandleNetworkMessage, this, std::placeholders::_1)
    );
}
void PlayScene::Terminate() {
    AudioHelper::StopBGM(bgmId);
    AudioHelper::StopSample(deathBGMInstance);
    deathBGMInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    IScene::Terminate();
}
void PlayScene::Update(float deltaTime) {
    IScene::Update(deltaTime);

    NetWork::Instance().Service(0);

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
                Engine::LOG(Engine::INFO) << "Invalid map value: " << mapData[y][x];
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
        if (!preview)
            return;
        
        // Check if valid.
        bool valid = CheckSpaceValid(x, y);
        if (!valid) {
            Engine::Sprite *sprite;
            sprite->Rotation = 0;
            return;
        }
        
        // Get the soldier type before placing
        std::string soldierType = "";
        if (dynamic_cast<Ninja*>(preview)) soldierType = "ninja";
        else if (dynamic_cast<Vikin*>(preview)) soldierType = "vikin";
        else if (dynamic_cast<Master*>(preview)) soldierType = "master";
        else if (dynamic_cast<Shooter*>(preview)) soldierType = "shooter";
        else if (dynamic_cast<Slime*>(preview)) soldierType = "slime";
        else if (dynamic_cast<Dragen*>(preview)) soldierType = "dragen";
        
        // Place soldier locally
        preview->GetObjectIterator()->first = false;
        UIGroup->RemoveObject(preview->GetObjectIterator());
        preview->Position.x = x * BlockSize + BlockSize / 2;
        preview->Position.y = y * BlockSize + BlockSize / 2;
        preview->Enabled = true;
        preview->Preview = false;
        preview->Tint = al_map_rgba(255, 255, 255, 255);
        
        // Assign unique ID and store
        uint32_t soldierId = nextSoldierId++;
        networkSoldiers[soldierId] = preview;
        
        SoldierGroup->AddNewObject(preview);
        preview->Update(0);
        
        // Send placement to other players
        SendSoldierPlacement(soldierType, x, y);
        
        // Update local item count
        if (LocalItemCount.count(soldierType)) {
            LocalItemCount[soldierType].first--;
            LocalItemCount[soldierType].second->Text = "x" + std::to_string(LocalItemCount[soldierType].first);
        }
        
        preview = nullptr;
        OnMouseMove(mx, my);
    }
}

void PlayScene::SendSoldierPlacement(const std::string& type, int x, int y) {
    if (!NetWork::Instance().isConnected()) return;
    
    PacketHeader header;
    header.type = MSG_PLACE_SOLDIER;
    header.length = sizeof(PacketHeader) + sizeof(PlaceSoldier);
    
    PlaceSoldier msg;
    msg.playerId = NetWork::Instance().myId;
    msg.soldierType = GetSoldierTypeId(type);
    msg.x = x;
    msg.y = y;
    msg.soldierId = nextSoldierId - 1; // Use the ID we just assigned
    
    // Send header + message
    std::vector<uint8_t> packet(header.length);
    memcpy(packet.data(), &header, sizeof(PacketHeader));
    memcpy(packet.data() + sizeof(PacketHeader), &msg, sizeof(PlaceSoldier));
    
    NetWork::Instance().Send(packet.data(), packet.size());
}

void PlayScene::HandleNetworkMessage(const ENetEvent& event) {
    if (event.type != ENET_EVENT_TYPE_RECEIVE) return;
    
    if (event.packet->dataLength < sizeof(PacketHeader)) return;
    
    PacketHeader header;
    memcpy(&header, event.packet->data, sizeof(PacketHeader));
    
    switch (header.type) {
        case MSG_PLACE_SOLDIER: {
            if (event.packet->dataLength >= sizeof(PacketHeader) + sizeof(PlaceSoldier)) {
                PlaceSoldier msg;
                memcpy(&msg, event.packet->data + sizeof(PacketHeader), sizeof(PlaceSoldier));
                
                // Don't process our own messages
                if (msg.playerId != NetWork::Instance().myId) {
                    CreateNetworkSoldier(msg.playerId, msg.soldierType, msg.x, msg.y, msg.soldierId);
                }
            }
            break;
        }
        case MSG_REMOVE_SOLDIER: {
            if (event.packet->dataLength >= sizeof(PacketHeader) + sizeof(RemoveSoldier)) {
                RemoveSoldier msg;
                memcpy(&msg, event.packet->data + sizeof(PacketHeader), sizeof(RemoveSoldier));
                
                // Remove soldier if it exists
                auto it = networkSoldiers.find(msg.soldierId);
                if (it != networkSoldiers.end()) {
                    SoldierGroup->RemoveObject(it->second->GetObjectIterator());
                    networkSoldiers.erase(it);
                }
            }
            break;
        }
        // Handle other message types...
    }
}

void PlayScene::CreateNetworkSoldier(uint8_t playerId, uint8_t soldierType, int x, int y, uint32_t soldierId) {
    Soldier* soldier = nullptr;
    
    switch (soldierType) {
        case 0: soldier = new Ninja(0, 0); break;
        case 1: soldier = new Vikin(0, 0); break;
        case 2: soldier = new Master(0, 0); break;
        case 3: soldier = new Shooter(0, 0); break;
        case 4: soldier = new Slime(0, 0); break;
        case 5: soldier = new Dragen(0, 0); break;
        default: return;
    }
    
    // Position the soldier
    soldier->Position.x = x * BlockSize + BlockSize / 2;
    soldier->Position.y = y * BlockSize + BlockSize / 2;
    soldier->Enabled = true;
    soldier->Preview = false;
    
    // Different visual indicator for enemy soldiers (optional)
    bool isEnemySoldier = (playerId != NetWork::Instance().myId);
    
    if (isEnemySoldier) {
    soldier->direction = 1; // Move right to left for enemy
    soldier->Tint = al_map_rgba(255, 100, 100, 255);
    soldier->Rotation = 180;
    } else {
    soldier->direction = -1; // Move left to right for your soldiers
    soldier->Tint = al_map_rgba(255, 255, 255, 255);
    }
    
    // Store and add to scene
    networkSoldiers[soldierId] = soldier;
    SoldierGroup->AddNewObject(soldier);
}

uint8_t PlayScene::GetSoldierTypeId(const std::string& type) {
    if (type == "ninja") return 0;
    if (type == "vikin") return 1;
    if (type == "master") return 2;
    if (type == "shooter") return 3;
    if (type == "slime") return 4;
    if (type == "dragen") return 5;
    return 0;
}

std::string PlayScene::GetSoldierTypeString(uint8_t typeId) {
    switch (typeId) {
        case 0: return "ninja";
        case 1: return "vikin";
        case 2: return "master";
        case 3: return "shooter";
        case 4: return "slime";
        case 5: return "dragen";
        default: return "ninja";
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
