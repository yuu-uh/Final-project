#include <allegro5/allegro.h>
#include <memory>

#include "Item.hpp"
#include "Engine/Point.hpp"
#include "Engine/Sprite.hpp"
#include "Engine/GameEngine.hpp"
#include "Engine/NetWork.hpp"
#include "Engine/Message.hpp"
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

    auto& net = NetWork::Instance();
    PacketHeader hdr;
    hdr.type   = MSG_PICK_ITEM;
    hdr.length = sizeof(PickItem);

    PickItem pi;
    pi.playerId = net.myId;
    pi.itemId   = this->id;

    uint8_t buf[ sizeof(hdr) + sizeof(pi) ];
    std::memcpy(buf, &hdr, sizeof(hdr));
    std::memcpy(buf + sizeof(hdr), &pi, sizeof(pi));
    net.Send(buf, sizeof(buf));

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



