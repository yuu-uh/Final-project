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
    bool picked;
    float float_timer;
    std::vector<Item*> allItems; 
    
public:
    Item(std::string img, float x, float y, std::string t);
    void Pick();
    bool item_picked();
    const std::string getBitmap();
    void Update(float deltaTime) override;
    bool Enabled = true;
    bool Preview = false;
    int id = -1;
    std::string getType(){ return type; }
    // void Draw() const override;
};
#endif