#ifndef SOLDIER_HPP
#define SOLDIER_HPP
#include <list>
#include <string>
#include <vector>

#include "Engine/Point.hpp"
#include "Engine/Sprite.hpp"

class Bullet;
class PlayScene;
class Turret;

enum State{
    walking, attacking
};

class Soldier : public Engine::Sprite {
protected:
    std::vector<Engine::Point> path;
    float speed;
    float hp;
    float dmg;
    float float_timer;
    int money;
    PlayScene *getPlayScene();
    virtual void die();
    State state;

    

public:
    float reachEndTime;
    bool Enabled = true;
    bool Preview = false;
    Soldier(std::string img, float x, float y, float radius, float speed, float hp, float dmg);
    void Hit(float damage);
    void UpdatePath(const std::vector<std::vector<int>> &mapDistance);
    virtual void Update(float deltaTime) override;
    void Draw() const override;
    float getSpeed(){ return speed; }
    void setSpeed(float s){ speed = s; }
    virtual std::string soldierName() const = 0;
};
#endif   // ENEMY_HPP
