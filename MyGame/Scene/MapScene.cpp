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
#include "Engine/IScene.hpp"
#include "Engine/Point.hpp"
#include "Engine/Resources.hpp"
#include "Engine/LOG.hpp"
#include "Scene/MapScene.hpp"
#include "UI/Component/ImageButton.hpp"
#include "UI/Component/Label.hpp"
#include "UI/Component/Slider.hpp"

const int MapScene::MapWidth = 20, MapScene::MapHeight = 13;
const int MapScene::BlockSize = 64;
void MapScene::Initialize() {
    int w = Engine::GameEngine::GetInstance().GetScreenSize().x;
    int h = Engine::GameEngine::GetInstance().GetScreenSize().y;
    int halfW = w / 2;
    int halfH = h / 2;
    remainTime = 180.0f;

    AddNewObject(TileMapGroup = new Group());
    AddNewObject(GroundEffectGroup = new Group());
    
    // Should support buttons.
    AddNewControlObject(UIGroup = new Group());
    ReadMap();
}
void MapScene::Update(float deltaTime) {
    
}
void MapScene::Terminate() {
    IScene::Terminate();
}
void MapScene::Draw() const{
    IScene::Draw();
}
void MapScene::OnMouseDown(int button, int mx, int my) {
    
}
void MapScene::OnMouseMove(int mx, int my) {
    
    const int x = mx / BlockSize;
    const int y = my / BlockSize;
    if (x < 0 || x >= MapWidth || y < 0 || y >= MapHeight) {
        imgTarget->Visible = false;
        return;
    }
    imgTarget->Visible = true;
    imgTarget->Position.x = x * BlockSize;
    imgTarget->Position.y = y * BlockSize;
}
void MapScene::OnMouseUp(int button, int mx, int my) {
    IScene::OnMouseUp(button, mx, my);
    if (!imgTarget->Visible)
        return;
    const int x = mx / BlockSize;
    const int y = my / BlockSize;
    if (button & 1) {
        if (mapState[y][x] != TILE_OCCUPIED) {
        
        }
    }
}
void MapScene::OnKeyDown(int keyCode) {
    IScene::OnKeyDown(keyCode);
    
}
void MapScene::ReadMap() {
    std::string filename = std::string("Resource/Map.txt");
    // Read map file.
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
    // Validate map data.
    if (static_cast<int>(mapData.size()) != MapWidth * MapHeight)
        throw std::ios_base::failure("Map data.");
    // Store map in 2d array.
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
void MapScene::ConstructUI() {
    
}

void MapScene::UIBtnClicked(int id) {
    
}

bool MapScene::CheckSpaceValid(int x, int y) {
    
}

