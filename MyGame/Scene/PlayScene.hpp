#ifndef PLAYSCENE_HPP
#define PLAYSCENE_HPP
#include <allegro5/allegro_audio.h>
#include <list>
#include <memory>
#include <utility>
#include <vector>

#include "Engine/IScene.hpp"
#include "Engine/Point.hpp"
#include "Items/Item.hpp"
#include "Soldier/Soldier.hpp"
#include "Engine/Message.hpp"
#include "Engine/NetWork.hpp"

class Turret;

namespace Engine {
    class Group;
    class Image;
    class Label;
    class Sprite;
}   // namespace Engine

class PlayScene final : public Engine::IScene {
private:
    enum TileType {
        TILE_DIRT,
        TILE_FLOOR,
        TILE_OCCUPIED,
    };
    ALLEGRO_SAMPLE_ID bgmId;
    std::shared_ptr<ALLEGRO_SAMPLE_INSTANCE> deathBGMInstance;
    float timer;
    Engine::Label* countdownLabel;
    Engine::Label* UILives;
    Engine::Image* MouseOnIcon = nullptr;
    int MouseIdx = -1;
    std::vector<std::shared_ptr<Item>> worldItems;
    std::vector<std::string> inventoryImgs;
    std::vector<Item*> inventory;
    std::unordered_map<uint32_t, Soldier*> networkSoldiers;  // Track soldiers by ID
    uint32_t nextSoldierId;  // Counter for unique soldier IDs
    uint8_t opponentLives = 10;  // Track opponent's lives
    bool gameEnded = false;      // Prevent multiple game end calls
    
    // Game result data for result scene
    struct GameResultData {
        uint8_t winnerId;
        uint8_t player1Lives;
        uint8_t player2Lives;
        uint8_t endReason;  // 0 = lives depleted, 1 = timeout
    };

    float soldierUpdateTimer;
    uint32_t gameStartTime;
    
    // New methods
    void SendSoldierUpdates();
    void SendSoldierUpdate(uint32_t soldierId, Soldier* soldier);
    void SendSoldierAttack(uint32_t soldierId, uint8_t targetType, uint32_t targetId = 0);
    void HandleSoldierUpdate(const SoldierUpdate& update);
    void HandleSoldierAttack(const SoldierAttack& attack);
    uint32_t GetGameTime() const;
protected:

public:
    static GameResultData lastGameResult; 
    int lives;
    std::vector<std::vector<int>> mapData;
    std::vector<std::pair<int, int>> GoalTiles;
    static const std::vector<Engine::Point> directions;
    static const std::vector<std::string> itemImg;
    static const int MapWidth, MapHeight;
    static const int BlockSize;
    float ticks;
    float deathCountDown;
    // Map tiles.
    Group *TileMapGroup;
    Group *GroundEffectGroup;
    Group *EffectGroup;
    Group *DebugIndicatorGroup;
    Group *BulletGroup;
    Group *TowerGroup;
    Group *SoldierGroup;
    Group *EnemyGroup;
    Group *UIGroup;
    Group *ItemGroup;
    Group *UIInventoryGroup;
    Engine::Label *livelabel;
    Engine::Image *imgTarget;
    Engine::Sprite *dangerIndicator;
    Soldier *preview;
    std::vector<std::vector<TileType>> mapState;
    std::vector<std::vector<int>> mapDistance;
    std::list<std::pair<int, float>> enemyWaveData;
    std::list<int> keyStrokes;
    static Engine::Point GetClientSize();
    explicit PlayScene() = default;
    void Initialize() override;
    void Terminate() override;
    void Update(float deltaTime) override;
    void Draw() const override;
    void OnMouseDown(int button, int mx, int my) override;
    void OnMouseMove(int mx, int my) override;
    void OnMouseUp(int button, int mx, int my) override;
    void Hit();
    void ReadMap();
    void ConstructUI();
    void UIBtnClicked(std::string type);
    bool CheckSpaceValid(int x, int y);
    MapScene *getMapScene();
    std::unordered_map<std::string, std::pair<int, Engine::Label*>> LocalItemCount;
    // void ModifyReadMapTiles();
    void SendSoldierPlacement(const std::string& type, int x, int y);
    void HandleNetworkMessage(const ENetEvent& event);
    void CreateNetworkSoldier(uint8_t playerId, uint8_t soldierType, int x, int y, uint32_t soldierId);
    uint8_t GetSoldierTypeId(const std::string& type);
    std::string GetSoldierTypeString(uint8_t typeId);
    Soldier* CreateSoldierByType(const std::string& typeName, int x, int y, int dir);

    // Network methods
    void SendCastleDamage(int damage = 1);
    void SendGameEnd(uint8_t winnerId, uint8_t reason);
    void HandleCastleDamage(uint8_t attackingPlayerId, uint8_t damage);
    void HandleGameEnd(const GameEnd& gameEnd);
};
#endif   // PLAYSCENE_HPP
