#ifndef ITEM_HPP
#define ITEM_HPP
#include <list>
#include <string>
#include <vector>

#include "Engine/Point.hpp"
#include "Engine/Sprite.hpp"

class MapScene;

class Item : public Engine::Sprite {
protected:
    MapScene *getMapScene();
    std::string type;
    bool picked;

public:
    Item(std::string img, float x, float y, std::string t);
    void Picked(float damage);
    // void Update(float deltaTime) override;
    // void Draw() const override;
};





#endif