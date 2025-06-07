#ifndef MAPSCENE_HPP
#define MAPSCENE_HPP
#include <allegro5/allegro_audio.h>
#include <list>
#include <memory>
#include <utility>
#include <vector>

#include "Engine/IScene.hpp"
#include "UI/Component/Label.hpp"
#include "Player/Player.hpp"
#include "Items/Item.hpp"

namespace Engine {
    class Group;
    class Image;
    class Label;
    class Sprite;
}   // namespace Engine

class MapScene final : public Engine::IScene {
private:
    enum TileType {
        TILE_DIRT,
        TILE_FLOOR,
        TILE_OCCUPIED,
    };
    ALLEGRO_SAMPLE_ID bgmId;
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> deathBGMInstance;
    int remainTime;
    Player *player;
    std::vector<Item*> inventory;   
    float camX = 0, camY = 0; 

public:
    static const std::vector<Engine::Point> directions;
    static const std::vector<std::string> itemImg;
    static const int MapWidth, MapHeight;
    static const int BlockSize;
    static const float DangerTime;
    static const Engine::Point SpawnGridPoint;
    static const Engine::Point EndGridPoint;
    static const std::vector<int> code;
    int MapId;
    float ticks;
    float deathCountDown;
    // Map tiles.
    Group *TileMapGroup;
    Group *GroundEffectGroup;
    Group *UIGroup;
    Group *ItemGroup;
    Group *UIInventoryGroup;
    Engine::Image *imgTarget;
    std::vector<std::vector<TileType>> mapState;
    std::vector<std::vector<int>> mapDistance;
    std::list<int> keyStrokes;
    static Engine::Point GetClientSize();
    explicit MapScene() = default;
    void Initialize() override;
    void Terminate() override;
    void Update(float deltaTime) override;
    void Draw() const override;
    void OnKeyDown(int keyCode) override;
    void ReadMap();
    void ConstructUI();
    void UIBtnClicked(int id);
    void PickupItem(Item* item);
    // bool CheckSpaceValid(int x, int y);
};

#endif   
