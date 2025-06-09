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
#include "UI/Animation/ExplosionEffect.hpp"
#include "UI/Animation/DirtyEffect.hpp"
#include "Scene/MapScene.hpp"
#include "Soldier/Soldier.hpp"
#include "Soldier/ninja.hpp"
#include "Soldier/master.hpp"
#include "Soldier/dragen.hpp"
#include "Soldier/shooter.hpp"
#include "Soldier/slime.hpp"
#include "Soldier/vikin.hpp"
#include "Engine/Message.hpp"


PlayScene::GameResultData PlayScene::lastGameResult = {0, 0, 0, 0};
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
    AddNewObject(EffectGroup = new Group());
    AddNewObject(ItemGroup = new Group);
    AddNewObject(SoldierGroup = new Group());
    AddNewControlObject(UIGroup = new Group());
    AddNewControlObject(UIInventoryGroup = new Group());
    AddNewObject(EnemyGroup = new Group());
    ReadMap();

    ConstructUI();
    lives = 10;

    timer = 300.0f;
    countdownLabel = new Engine::Label("05:00", "pirulen.ttf", 48,
    Engine::GameEngine::GetInstance().GetScreenSize().x - 120,
    Engine::GameEngine::GetInstance().GetScreenSize().y - 40, 255, 255, 255, 255, 0.5, 0.5);
    UIGroup->AddNewObject(countdownLabel);

    livelabel = new Engine::Label(
        std::string("Life ") + std::to_string(lives),
        "pirulen.ttf", 48, 1410, 50, 255,255,255,255, 0.5,0.5);
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
    NetWork::Instance().SetReceiveCallback(nullptr);
    
    // AudioHelper::StopBGM(bgmId);
    // AudioHelper::StopSample(deathBGMInstance);
    // deathBGMInstance = std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE>();
    IScene::Terminate();
}
void PlayScene::Update(float deltaTime) {
    if(gameEnded) return;

    IScene::Update(deltaTime);

    NetWork::Instance().Service(0);

    timer -= deltaTime;
    if (timer <= 0) {
        uint8_t winnerId = 0;  // Draw by default
        if (lives > opponentLives) {
            winnerId = NetWork::Instance().myId;
        } else if (opponentLives > lives) {
            winnerId = (NetWork::Instance().myId == 1) ? 2 : 1;
        }
        
        SendGameEnd(winnerId, 1);  // 1 = ended due to timeout
        
        // Store result data
        lastGameResult.winnerId = winnerId;
        lastGameResult.player1Lives = (NetWork::Instance().myId == 1) ? lives : opponentLives;
        lastGameResult.player2Lives = (NetWork::Instance().myId == 2) ? lives : opponentLives;
        lastGameResult.endReason = 1;
        
        gameEnded = true;
        Engine::GameEngine::GetInstance().ChangeScene("result");
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
    EnemyGroup->Update(deltaTime);
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
        for (int x = 0; x < MapWidth; x++){
            mapData[y][x] = line[x] - '0';
            if(mapData[y][x] == 4)
                GoalTiles.emplace_back(x, y);
        }
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
            case 4:
            case 5:
            case 6:
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
    EnemyGroup->Draw();
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
    //if(mapData[y][x] != 5) return;
    if (button & 1) {
        if (!preview)
            return;
        
        // Check if valid.
        bool valid = CheckSpaceValid(x, y);
        if (!valid) {
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
    if(gameEnded) return;
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
    if(gameEnded) return;
    if (event.type != ENET_EVENT_TYPE_RECEIVE) return;
    
    if (event.packet->dataLength < sizeof(PacketHeader)) return;
    
    PacketHeader header;
    memcpy(&header, event.packet->data, sizeof(PacketHeader));
    
    switch (header.type) {
        case MSG_PLACE_SOLDIER: {
            if (event.packet->dataLength >= sizeof(PacketHeader) + sizeof(PlaceSoldier)) {
                PlaceSoldier msg;
                memcpy(&msg, event.packet->data + sizeof(PacketHeader), sizeof(PlaceSoldier));
                std::cout<<msg.playerId<<std::endl;
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
        case MSG_CASTLE_DAMAGE: {
            if (event.packet->dataLength >= sizeof(PacketHeader) + sizeof(CastleDamage)) {
                CastleDamage msg;
                memcpy(&msg, event.packet->data + sizeof(PacketHeader), sizeof(CastleDamage));
                HandleCastleDamage(msg.attackingPlayerId, msg.damage);
            }
            break;
        }
        case MSG_GAME_END: {
            if (event.packet->dataLength >= sizeof(PacketHeader) + sizeof(GameEnd)) {
                GameEnd msg;
                memcpy(&msg, event.packet->data + sizeof(PacketHeader), sizeof(GameEnd));
                HandleGameEnd(msg);
            }
            break;
        }
        // Handle other message types...
    }
}

void PlayScene::CreateNetworkSoldier(uint8_t playerId, uint8_t soldierType, int x, int y, uint32_t soldierId) {
    if(gameEnded || !EnemyGroup) return;
    int realX = PlayScene::MapWidth - 1 - x;
    int realY = y;

    std::string typeName = GetSoldierTypeString(soldierType);
    Soldier* soldier = CreateSoldierByType(typeName, realX, realY, -1);
    if (!soldier) return;

    soldier->Position.x = realX * BlockSize + BlockSize / 2;
    soldier->Position.y = realY * BlockSize + BlockSize / 2;
    soldier->Enabled = true;
    soldier->Preview = false;
    soldier->Tint = al_map_rgba(255, 100, 100, 255);
    networkSoldiers[soldierId] = soldier;
    EnemyGroup->AddNewObject(soldier);
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

Soldier* PlayScene::CreateSoldierByType(const std::string& typeName, int x, int y, int dir) {
    if (typeName == "ninja") {
        return new Ninja(x, y, dir);
    } else if (typeName == "vikin") {
        return new Vikin(x, y, dir);
    } else if (typeName == "master") {
        return new Master(x, y, dir);
    } else if (typeName == "shooter") {
        return new Shooter(x, y, dir);
    } else if (typeName == "slime") {
        return new Slime(x, y, dir);
    } else if (typeName == "dragen") {
        return new Dragen(x, y, dir);
    }
    return nullptr; // Unknown type
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

void PlayScene::SendCastleDamage(int damage) {
    if (!NetWork::Instance().isConnected() || gameEnded) return;
    
    PacketHeader header;
    header.type = MSG_CASTLE_DAMAGE;
    header.length = sizeof(PacketHeader) + sizeof(CastleDamage);
    
    CastleDamage msg;
    msg.attackingPlayerId = NetWork::Instance().myId;
    msg.damage = damage;
    
    std::vector<uint8_t> packet(header.length);
    memcpy(packet.data(), &header, sizeof(PacketHeader));
    memcpy(packet.data() + sizeof(PacketHeader), &msg, sizeof(CastleDamage));
    
    NetWork::Instance().Send(packet.data(), packet.size());
}

void PlayScene::SendGameEnd(uint8_t winnerId, uint8_t reason) {
    if (!NetWork::Instance().isConnected()) return;
    
    PacketHeader header;
    header.type = MSG_GAME_END;
    header.length = sizeof(PacketHeader) + sizeof(GameEnd);
    
    GameEnd msg;
    msg.winnerId = winnerId;
    msg.player1Lives = (NetWork::Instance().myId == 1) ? lives : opponentLives;
    msg.player2Lives = (NetWork::Instance().myId == 2) ? lives : opponentLives;
    msg.reason = reason;
    
    std::vector<uint8_t> packet(header.length);
    memcpy(packet.data(), &header, sizeof(PacketHeader));
    memcpy(packet.data() + sizeof(PacketHeader), &msg, sizeof(GameEnd));
    
    NetWork::Instance().Send(packet.data(), packet.size());
}

void PlayScene::HandleCastleDamage(uint8_t attackingPlayerId, uint8_t damage) {
    if (attackingPlayerId != NetWork::Instance().myId && !gameEnded) {
        // Opponent's soldier damaged our castle
        Hit();
    }
}

void PlayScene::HandleGameEnd(const GameEnd& gameEnd) {
    if (gameEnded) return;  // Already handled
    
    gameEnded = true;
    
    // Store the result data
    lastGameResult.winnerId = gameEnd.winnerId;
    lastGameResult.player1Lives = gameEnd.player1Lives;
    lastGameResult.player2Lives = gameEnd.player2Lives;
    lastGameResult.endReason = gameEnd.reason;
    
    // Update our local state with opponent's final lives
    opponentLives = (NetWork::Instance().myId == 1) ? gameEnd.player2Lives : gameEnd.player1Lives;
    
    Engine::GameEngine::GetInstance().ChangeScene("result");
}

void PlayScene::Hit() {
    // lives--;
    // Engine::LOG(Engine::INFO)<<"lives: "<<lives;
    // UILives->Text = std::string("Life ") + std::to_string(lives);
    // if (lives <= 0) {
    //     Engine::GameEngine::GetInstance().ChangeScene("result");
    // }
    if (gameEnded) return;  // Prevent multiple hits after game ends
    
    lives--;
    Engine::LOG(Engine::INFO) << "lives: " << lives;
    livelabel->Text = std::string("Life ") + std::to_string(lives);
    
    if (lives <= 0) {
        // Game over - this player lost
        uint8_t winnerId = (NetWork::Instance().myId == 1) ? 2 : 1;  // Opponent wins
        SendGameEnd(winnerId, 0);  // 0 = ended due to lives
        
        // Store result data
        lastGameResult.winnerId = winnerId;
        lastGameResult.player1Lives = (NetWork::Instance().myId == 1) ? lives : opponentLives;
        lastGameResult.player2Lives = (NetWork::Instance().myId == 2) ? lives : opponentLives;
        lastGameResult.endReason = 0;
        
        gameEnded = true;
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
        next_preview = new Ninja(0, 0, 1);
    }else if(type == "vikin"){
        next_preview = new Vikin(0, 0, 1);
    }else if(type == "master"){
        next_preview = new Master(0, 0, 1);
    }else if(type == "shooter"){
        next_preview = new Shooter(0, 0, 1);
    }else if(type == "slime"){
        next_preview = new Slime(0, 0, 1);
    }else if(type == "dragen"){
        next_preview  = new Dragen(0, 0, 1);
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