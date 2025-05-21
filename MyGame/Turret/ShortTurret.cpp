#include <allegro5/base.h>
#include <cmath>
#include <string>

#include "Bullet/LaserBullet.hpp"
#include "Engine/AudioHelper.hpp"
#include "Engine/Group.hpp"
#include "Engine/Point.hpp"
#include "ShortTurret.hpp"
#include "Scene/PlayScene.hpp"

const int ShortTurret::Price = 350;
ShortTurret::ShortTurret(float x, float y) : Turret("play/tower-base.png", "play/explosion-3.png", x, y, 500, Price, 0.5) {
    // Move center downward, since we the turret head is slightly biased upward.
    Anchor.y += 8.0f / GetBitmapHeight();
}
void ShortTurret::CreateBullet() {
    Engine::Point diff = Engine::Point(cos(Rotation - ALLEGRO_PI / 2), sin(Rotation - ALLEGRO_PI / 2));
    float rotation = atan2(diff.y, diff.x);
    Engine::Point normalized = diff.Normalize();
    Engine::Point normal = Engine::Point(-normalized.y, normalized.x);
    // Change bullet position to the front of the gun barrel.
    for(int i = 0; i < 9; i++){
        getPlayScene()->BulletGroup->AddNewObject(new LaserBullet(Position + normalized * 36, Engine::Point(cos(i*ALLEGRO_PI/(4.5)), sin(i*ALLEGRO_PI/(4.5))), rotation, this));
    }
    AudioHelper::PlayAudio("explosion.wav");
}
