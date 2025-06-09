#ifndef SOLDIER_HPP
#define SOLDIER_HPP
#include <list>
#include <string>
#include <vector>

#include "Engine/Point.hpp"
#include "Engine/Sprite.hpp"
#include "Engine/Group.hpp"

class Bullet;
class PlayScene;
class Turret;

enum State{
    walking, attacking, idle, dead
};

class Soldier : public Engine::Sprite {
protected:
    bool AttackCastle = false;
    int tileX, tileY;
    int baseY;
    float attackRadius;
    float cooldown;
    float attackTimer = 0.0f;
    std::vector<Engine::Point> path;
    float speed;
    float hp;
    float maxHp;
    float dmg;
    float float_timer;
    int money;
    PlayScene *getPlayScene();
    virtual void die();
    State state;
    Soldier* target = nullptr;
    Engine::Group* Enemy;

    

public:
    bool isLocal;  
    float reachEndTime;
    int direction = 1;
    bool Enabled = true;
    bool Preview = false;
    Soldier(std::string img, float x, float y, int dir, float radius, float speed, float hp, float dmg, bool isLocal);
    void Hit(float damage);
    void UpdatePath(const std::vector<std::vector<int>> &mapDistance);
    virtual void Update(float deltaTime) override;
    void Draw() const override;
    float getSpeed(){ return speed; }
    void setSpeed(float s){ speed = s; }
    virtual std::string soldierName() const = 0;
    virtual void Attack();
};
#endif   // ENEMY_HPP
