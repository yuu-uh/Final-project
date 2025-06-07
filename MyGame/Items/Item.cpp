#include <allegro5/allegro.h>
#include <memory>

#include "Item.hpp"
#include "Engine/Point.hpp"
#include "Engine/Sprite.hpp"
#include "Engine/GameEngine.hpp"
#include "Scene/MapScene.hpp"
#include "UI/Component/Image.hpp"

class MapScene;

MapScene *Item::getMapScene(){
    return dynamic_cast<MapScene*>(Engine::GameEngine::GetInstance().GetActiveScene());
}

Item::Item(std::string img, float x, float y, std::string t):Sprite(img, x, y, 30, 30), image_name(img), type(t), picked(false){
    CollisionRadius = 5;
    float_timer = 0.0f;
}
void Item::Pick(){
    if(picked) return;
    picked = true;
    Visible = false;
    getMapScene()->PickupItem(this, this->type);
}

void Item::Update(float deltaTime) {
    float_timer += deltaTime;
    Position.y += (std::sin(float_timer*4.0f) / 6); // oscillates ±5px
    Sprite::Update(deltaTime); // update animation if needed
    return;
}

bool Item::item_picked(){
    return picked;
}

const std::string Item::getBitmap(){
    return image_name;
 }



