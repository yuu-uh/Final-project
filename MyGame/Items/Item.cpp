#include <allegro5/allegro.h>
#include <memory>

#include "Item.hpp"
#include "Engine/Point.hpp"
#include "Engine/Sprite.hpp"
#include "UI/Component/Image.hpp"

Item::Item(std::string img, float x, float y, std::string t):Sprite(img, x, y, 30, 30), image_name(img), type(t), picked(false){
    CollisionRadius = 5;
}
void Item::Picked(float damage){
    if(picked) return;
    picked = true;
    Visible = false;
    
}

bool Item::item_picked(){
    return picked;
}

const std::string Item::getBitmap(){
    return image_name;
 }



