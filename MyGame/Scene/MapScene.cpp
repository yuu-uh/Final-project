#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_primitives.h> 
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <tuple>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <random>

#include "Engine/Message.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/IScene.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "Engine/LOG.hpp"
#include "Engine/Collider.hpp"
#include "Engine/NetWork.hpp"
#include "Player/Player.hpp"
#include "Items/Item.hpp"
#include "Scene/MapScene.hpp"
#include "Scene/PlayScene.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/Slider.hpp"
#include "Items/Item.hpp"

const int MapScene::MapWidth = 25, MapScene::MapHeight = 20;
const int MapScene::BlockSize = 64;
const std::vector<std::string> MapScene::itemImg = {"ninja", "master", "slime", "vikin", "dragen", "shooter"};

void MapScene::Initialize() {
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;
    AddNewObject(TileMapGroup = new Group());
    AddNewObject(GroundEffectGroup = new Group());
    AddNewObject(ItemGroup = new Group);
    AddNewControlObject(UIGroup = new Group());
    AddNewControlObject(UIInventoryGroup = new Group());
    ReadMap();
    
    UIGroup->AddNewObject(new Engine::Image("GUI/frame.png",1280, 5,320,320));
    UIGroup->AddNewObject(new Engine::Image("GUI/inventory.png",1275, 320,325,377));
    UIGroup->AddNewObject(new Engine::Image("GUI/button.png",1275, 700, 320, 108));
    timer = 30.0f;
    countdownLabel = new Engine::Label("03:00", "pirulen.ttf", 48, 1380, 730, 255, 255, 255, 255, 0.5, 0.5);
    UIGroup->AddNewObject(countdownLabel);
    
    std::vector<std::pair<int,int>> freeCells;
    freeCells.reserve(MapWidth * MapHeight);
    for(int y=0; y<MapHeight; ++y){
        for(int x=0; x<MapWidth; ++x){
            if(mapState[y][x] != TILE_OCCUPIED)
                freeCells.emplace_back(x,y);
        }
    }
    std::mt19937 rng{ std::random_device{}() };
    std::shuffle(freeCells.begin(), freeCells.end(), rng);
    int nextId = 0;
    int needed = std::min((int)freeCells.size(), 6*5);
    for(int idx = 0; idx < needed; ++idx){
        auto [x, y] = freeCells[idx];
        std::string img = "mapScene/" + itemImg[idx/5] + ".png";  
        Item* item = new Item(img, x * BlockSize, y * BlockSize, itemImg[idx/5]);
        item->id = nextId++;
        allItems.push_back(item);
        ItemGroup->AddNewObject(item);
    }

    Engine::Point startPos1{ 216, 216 };   
    Engine::Point startPos2{ 16, 16 };    
    float moveSpeed = 200.0f;     
    player = new Player("mapScene/player1_front01.png", startPos1, moveSpeed, 16, 16, true);
    conPlayer  = new Player("mapScene/player1_front01.png", startPos2, moveSpeed, 16, 16, false);
    AddNewObject(player);
    AddNewObject(conPlayer);

    auto& net = NetWork::Instance();
    net.SetReceiveCallback([this,&net](const ENetEvent& ev){
        if (ev.type != ENET_EVENT_TYPE_RECEIVE) return;
        auto const* data = ev.packet->data;
        auto const* hdr  = reinterpret_cast<const PacketHeader*>(data);
        auto const* body = data + sizeof(PacketHeader);

        if (hdr->type == MSG_PLAYER_STATE && hdr->length == sizeof(PlayerState)) {
            auto const* ps = reinterpret_cast<const PlayerState*>(body);
            if (ps->playerId != net.myId) {
                conPlayer->Position.x = ps->x;
                conPlayer->Position.y = ps->y;
                conPlayer->SetAction(ps->action);
            }
        }
        else if (hdr->type == MSG_PICK_ITEM && hdr->length == sizeof(PickItem)) {
            auto const* pi = reinterpret_cast<const PickItem*>(body);
            if (pi->playerId == net.myId) {
                enet_packet_destroy(ev.packet);
                return;
            }
            for (auto* it : allItems) {
                if (it->id == pi->itemId && !it->item_picked()) {
                    it->Pick();
                    break;
                }
            }
        }
        enet_packet_destroy(ev.packet);
    }); 
}
void MapScene::Update(float dt) {
    IScene::Update(dt);
    auto& net = NetWork::Instance();
    net.Service(0);

    PlayerState ps;
    ps.playerId = net.myId;        
    ps.x        = player->Position.x;
    ps.y        = player->Position.y;
    ps.action   = player->CurrentAction();

    PacketHeader hdr;
    hdr.type   = MSG_PLAYER_STATE;
    hdr.length = sizeof(PlayerState);

    uint8_t buf[sizeof(hdr) + sizeof(ps)];
    std::memcpy(buf,                   &hdr, sizeof(hdr));
    std::memcpy(buf + sizeof(hdr),     &ps,  sizeof(ps));
    net.Send(buf, sizeof(buf), 0);

    float screenW = Engine::GameEngine::GetInstance().GetScreenSize().x;
    float screenH = Engine::GameEngine::GetInstance().GetScreenSize().y;
    const float panelW = 320;              
    const float viewW  = screenW - panelW;  
    const float viewH  = screenH;           
    float targetX = player->Position.x - viewW / 2.0f;
    float targetY = player->Position.y - viewH / 2.0f;
    camX = std::clamp(targetX, 0.0f, MapWidth*BlockSize - viewW);
    camY = std::clamp(targetY, 0.0f, MapHeight*BlockSize - viewH);
    for (auto& it : ItemGroup->GetObjects()) {
        auto item = dynamic_cast<Item*>(it);
        if (!item) continue; // Skip if not an Item

        if (!item->item_picked() && Engine::Collider::IsCircleOverlap(
            item->Position, item->CollisionRadius, player->Position, player->CollisionRadius)) {
            item->Pick();

            PacketHeader hdr{ MSG_PICK_ITEM, sizeof(PickItem) };
            PickItem pi{ uint8_t(net.myId), uint8_t(item->id) };
            uint8_t packet[sizeof(hdr)+sizeof(pi)];
            std::memcpy(packet, &hdr, sizeof(hdr));
            std::memcpy(packet + sizeof(hdr), &pi, sizeof(pi));
            net.Send(packet, sizeof(packet), 0);

            AddToInventory(item, item->getType());
        }
    }

    timer -= dt;
    if (timer <= 0) {
        PlayScene::player1 = this->player;
        PlayScene::player2 = this->conPlayer;
        Engine::GameEngine::GetInstance().ChangeScene("play");
        return;
    }
    int minutes = int(timer) / 60;
    int seconds = int(timer) % 60;
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << minutes
        << ":" << std::setw(2) << std::setfill('0') << seconds;
    countdownLabel->Text = oss.str();
}
void MapScene::Terminate() {
    IScene::Terminate();
}
void MapScene::Draw() const {
    IScene::Draw();
    al_clear_to_color(al_map_rgb(0,0,0));

    ALLEGRO_TRANSFORM oldX;
    al_copy_transform(&oldX, al_get_current_transform());

    ALLEGRO_TRANSFORM cam;
    al_identity_transform(&cam);
    al_translate_transform(&cam, -camX, -camY);
    al_use_transform(&cam);
    float screenW = Engine::GameEngine::GetInstance().GetScreenSize().x;
    float screenH = Engine::GameEngine::GetInstance().GetScreenSize().y;
    const float panelW = 320;
    float viewW = screenW - panelW;
    float viewH = screenH;
    int colStart = std::max(0, int(camX / BlockSize));
    int rowStart = std::max(0, int(camY / BlockSize));
    int colEnd   = std::min(MapWidth-1, int((camX + viewW) / BlockSize)+1);
    int rowEnd   = std::min(MapHeight-1, int((camY + viewH) / BlockSize)+1);
    TileMapGroup->Draw();
    ItemGroup->Draw();
    player->Draw();
    conPlayer->Draw();

    al_use_transform(&oldX);
    // package
    al_draw_filled_rectangle(screenW - panelW, 0, screenW, screenH, al_map_rgb(0,0,0));
    UIGroup->Draw();
    UIInventoryGroup->Draw();

    // mini map
    const float miniW = 290, miniH = 290;
    const float miniX = screenW - miniW - 13, miniY = 21;
    al_draw_filled_rectangle(miniX, miniY, miniX+miniW, miniY+miniH, al_map_rgba(0,0,0,200));
    al_draw_rectangle(miniX, miniY, miniX+miniW, miniY+miniH, al_map_rgb(255,255,255), 2);

    for(int row=0; row<MapHeight; ++row){
        for(int col=0; col<MapWidth; ++col){
            float cellW = miniW / MapWidth;
            float cellH = miniH / MapHeight;
            ALLEGRO_COLOR c;
            switch (mapState[row][col]) {
                case TILE_FLOOR:
                    c = al_map_rgb(146, 218, 226);
                    break;
                case TILE_DIRT:
                    c = al_map_rgb(118, 166,  53);
                    break;
                case TILE_OCCUPIED:
                    c = al_map_rgb(112, 128,  138);
                    break;
                default:
                    c = al_map_rgb(100, 100, 100);
                    break;
            }
            al_draw_filled_rectangle(miniX + col*cellW, miniY + row*cellH,miniX + (col+1)*cellW, miniY + (row+1)*cellH,c);
        }
    }
    auto drawDot = [&](const Player* p, ALLEGRO_COLOR col){
        float px = p->Position.x / (MapWidth*BlockSize);
        float py = p->Position.y / (MapHeight*BlockSize);
        al_draw_filled_circle(
            miniX + px * miniW,
            miniY + py * miniH, 4, col
        );
    };
    drawDot(player,    al_map_rgb(255, 255, 0));
    drawDot(conPlayer, al_map_rgb(255, 50, 50));
}
void MapScene::OnKeyDown(int keyCode) {
    IScene::OnKeyDown(keyCode);
    UIInventoryGroup->Draw();  
}
void MapScene::ReadMap() {
    std::ifstream fin("Resource/Map.txt");
    if (!fin) throw std::runtime_error("无法打开 Resource/Map.txt");

    std::string line;
    mapState.assign(MapHeight, std::vector<TileType>(MapWidth));
    TileMapGroup->Clear();  // 清空旧的

    for (int y = 0; y < MapHeight; ++y) {
        if (!std::getline(fin, line) || (int)line.size() < MapWidth)
            throw std::runtime_error("Map.txt 第 " + std::to_string(y) + " 行长度不足");
        for (int x = 0; x < MapWidth; ++x) {
            char c = line[x];
            TileType t;
            const char* imgPath = nullptr;
            switch (c) {
                case '0':
                    t = TILE_FLOOR;
                    imgPath = "mapScene/stone.png";
                    break;
                case '1':
                    t = TILE_DIRT;
                    imgPath = "mapScene/grass.png";
                    break;
                case '2':
                    t = TILE_OCCUPIED;
                    imgPath = "mapScene/Tile.png";
                    break;
                default:
                    throw std::runtime_error(std::string("Map.txt 非法字符: ") + c);
            }
            mapState[y][x] = t;

            // 一次性把图元加入 TileMapGroup
            TileMapGroup->AddNewObject(
                new Engine::Image(imgPath,
                                  x * BlockSize,
                                  y * BlockSize,
                                  BlockSize,
                                  BlockSize)
            );
        }
    }
}
void MapScene::PickupItem(Item* item) {
     item->Pick();
 }
void MapScene::AddToInventory(Item* item, const std::string& type) {
    inventory.push_back(item);
    const int panelX0 = 1300, panelY0 = 320, pad = 30;
    const int iconW = 100, iconH = 100;
    int cols = 320 / (iconW + pad);

    if(Engine::GameEngine::GetInstance().itemCount.count(type)){
        //inventoryCount[type].first += 1;
        Engine::GameEngine::GetInstance().itemCount[type].first += 1;
        int newCount = Engine::GameEngine::GetInstance().itemCount[type].first;
        //inventoryCount[type].second->Text = "x" + std::to_string(newCount);
        Engine::GameEngine::GetInstance().itemCount[type].second->Text = "x" + std::to_string(newCount);
        return;
    }

    int idx = (int)Engine::GameEngine::GetInstance().itemCount.size();  
    int col = idx % cols;
    int row = idx / cols;
    float x = panelX0 + pad + col * (iconW + pad);
    float y = panelY0 + pad + row * (iconH + pad);

    Engine::Image* icon = new Engine::Image(item->getBitmap(), x, y, iconW, iconH);
    UIInventoryGroup->AddNewObject(icon);
    inventoryIcons[type] = icon;

    // Add label for count
    Engine::Label* countLabel = new Engine::Label("x1", "pirulen.ttf", 24,
        x + iconW - 10, y + iconH - 10, 255, 255, 255, 255, 1.0, 1.0);
    UIInventoryGroup->AddNewObject(countLabel);

    // Add to map
    //inventoryCount[type] = std::make_pair(1, countLabel);
    Engine::GameEngine::GetInstance().itemCount[type] = std::make_pair(1, countLabel);
    Engine::GameEngine::GetInstance().pickedItems.push_back(type);
}


void MapScene::ConstructUI() {
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
        auto empty = new Engine::Image(
        "mapScene/item_empty.png",
        x, y,
        iconW, iconH
        );
        UIInventoryGroup->AddNewObject(empty);
        slotImage.push_back(empty);
    }
}
void MapScene::UIBtnClicked(int id) {
    
}
bool MapScene::CheckSpaceValid(int x, int y){
    if(mapState[y][x] == TILE_OCCUPIED) return false;
    return true;
}
