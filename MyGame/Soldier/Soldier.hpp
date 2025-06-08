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
    walking, attacking
};

class Soldier : public Engine::Sprite {
protected:
    float attackRadius;
    float cooldown;
    float attackTimer = 0.0f;
    std::vector<Engine::Point> path;
    float speed;
    float hp;
    float dmg;
    float float_timer;
    int money;
    PlayScene *getPlayScene();
    virtual void die();
    State state;
    Engine::Group* enemyGroup;

    

public:
    float reachEndTime;
    int direction = 1;
    bool Enabled = true;
    bool Preview = false;
    Soldier(std::string img, float x, float y, int dir, float radius, float speed, float hp, float dmg);
    void Hit(float damage);
    void UpdatePath(const std::vector<std::vector<int>> &mapDistance);
    virtual void Update(float deltaTime) override;
    void Draw() const override;
    float getSpeed(){ return speed; }
    void setSpeed(float s){ speed = s; }
    virtual std::string soldierName() const = 0;
};
#endif   // ENEMY_HPP
