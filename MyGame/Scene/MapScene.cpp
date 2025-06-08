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
    
    timer = 30.0f;

    countdownLabel = new Engine::Label("03:00", "pirulen.ttf", 48,
    Engine::GameEngine::GetInstance().GetScreenSize().x - 120,
    Engine::GameEngine::GetInstance().GetScreenSize().y - 40, 255, 255, 255, 255, 0.5, 0.5);
    UIGroup->AddNewObject(countdownLabel);
    
    int nextId = 0;
    for(int i=0; i<6; i++){
        for(int j=0; j<5; j++){
            int x = rand() % MapWidth;
            int y = rand() % MapHeight;
            std::string img = "mapScene/"+itemImg[i]+".png";
            Item* item = new Item(img, x * BlockSize, y * BlockSize, itemImg[i]);
            item->id = nextId++;
            allItems.push_back(item);
            ItemGroup->AddNewObject(item);
        }
    }
    Engine::Point startPos1{ 216, 216 };   
    Engine::Point startPos2{ 16, 16 };    
    float moveSpeed = 200.0f;     
    player = new Player("mapScene/player1_front01.png", startPos1, moveSpeed, 16, 16);
    conPlayer  = new Player("mapScene/player1_front01.png", startPos2, moveSpeed, 16, 16);
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
            if (!item->item_picked()) {
                item->Pick(); // or some value
            }
        }
    }

    timer -= dt;
    if (timer <= 0) {
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
    float viewH = Engine::GameEngine::GetInstance().GetScreenSize().y;

    int colStart = std::max(0, int(camX / BlockSize));
    int rowStart = std::max(0, int(camY / BlockSize));
    int colEnd = std::min(MapWidth - 1, int((camX + viewW) / BlockSize) + 1);
    int rowEnd = std::min(MapHeight - 1, int((camY + viewH) / BlockSize) + 1);

    for (int row = rowStart; row <= rowEnd; ++row) {
        for (int col = colStart; col <= colEnd; ++col) {
            TileType t = mapState[row][col];
            const char* path = (t == TILE_FLOOR) ? "mapScene/stone.png" : "mapScene/grass.png";
            float x = col * BlockSize;
            float y = row * BlockSize;
            ALLEGRO_BITMAP* bmp = Engine::Resources::GetInstance().GetBitmap(path).get();
            al_draw_scaled_bitmap(
                bmp, 0, 0,
                al_get_bitmap_width(bmp), al_get_bitmap_height(bmp),
                x, y,
                BlockSize, BlockSize,
                0
            );
        }
    }

    ItemGroup->Draw();
    player->Draw();
    conPlayer->Draw();
    al_use_transform(&oldX);
    al_draw_filled_rectangle(screenW - panelW, 0, screenW, screenH, al_map_rgb(0, 0, 0));
    UIGroup->Draw();
    UIInventoryGroup->Draw();
}
void MapScene::OnKeyDown(int keyCode) {
    IScene::OnKeyDown(keyCode);
    UIInventoryGroup->Draw();  
}
void MapScene::ReadMap() {
    std::string filename = std::string("Resource/Map.txt");
    
    char c;
    std::vector<bool> mapData;
    std::ifstream fin(filename);
    while (fin >> c) {
        switch (c) {
            case '0': mapData.push_back(false); break;
            case '1': mapData.push_back(true); break;
            case '\n':
            case '\r':
                if (static_cast<int>(mapData.size()) / MapWidth != 0)
                    throw std::ios_base::failure("Map data is corrupted.");
                break;
            default: throw std::ios_base::failure("Map data is.");
        }
    }
    fin.close();

    if (static_cast<int>(mapData.size()) != MapWidth * MapHeight)
        throw std::ios_base::failure("Map data.");
    mapState = std::vector<std::vector<TileType>>(MapHeight, std::vector<TileType>(MapWidth));
    for (int i = 0; i < MapHeight; i++) {
        for (int j = 0; j < MapWidth; j++) {
            const int num = mapData[i * MapWidth + j];
            mapState[i][j] = num ? TILE_FLOOR : TILE_DIRT;
            if (num)
                TileMapGroup->AddNewObject(new Engine::Image("mapScene/stone.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
            else
                TileMapGroup->AddNewObject(new Engine::Image("mapScene/grass.png", j * BlockSize, i * BlockSize, BlockSize, BlockSize));
        }
    }
}
void MapScene::PickupItem(Item* item, std::string itemType) {
    inventory.push_back(item);

    const int panelX0 = 1300;
    const int panelY0 = 320;
    const int pad     = 30;
    const int iconW   = 100, iconH = 100;
    const int cols    = 320 / (iconW + pad);

    if(inventoryCount.count(itemType)){
        inventoryCount[itemType].first += 1;
        Engine::GameEngine::GetInstance().itemCount[itemType].first += 1;
        int newCount = inventoryCount[itemType].first;
        inventoryCount[itemType].second->Text = "x" + std::to_string(newCount);
        Engine::GameEngine::GetInstance().itemCount[itemType].second->Text = "x" + std::to_string(newCount);
        return;
    }

    int idx = (int)inventoryCount.size();  
    int col = idx % cols;
    int row = idx / cols;
    float x = panelX0 + pad + col * (iconW + pad);
    float y = panelY0 + pad + row * (iconH + pad);


    Engine::Image* icon = new Engine::Image(item->getBitmap(), x, y, iconW, iconH);
    UIInventoryGroup->AddNewObject(icon);
    inventoryIcons[itemType] = icon;

    // Add label for count
    Engine::Label* countLabel = new Engine::Label("x1", "pirulen.ttf", 24,
        x + iconW - 10, y + iconH - 10, 255, 255, 255, 255, 1.0, 1.0);
    UIInventoryGroup->AddNewObject(countLabel);

    // Add to map
    inventoryCount[itemType] = std::make_pair(1, countLabel);
    Engine::GameEngine::GetInstance().itemCount[itemType] = std::make_pair(1, countLabel);
    Engine::GameEngine::GetInstance().pickedItems.push_back(itemType);
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

//bool MapScene::CheckSpaceValid(int x, int y) {
//    return true;
//}

