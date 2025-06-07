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
    std::string image_name;
public:
    Item(std::string img, float x, float y, std::string t);
    void Picked(float damage);
    bool item_picked();
    bool picked;
    const std::string getBitmap();
    // void Update(float deltaTime) override;
    // void Draw() const override;
};





#endif