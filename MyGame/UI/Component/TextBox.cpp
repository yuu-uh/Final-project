#include <functional>
#include <string>
#include <memory>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>

#include "UI/Component/TextBox.hpp"
#include "Engine/Resources.hpp"

namespace Engine{
    TextBox::TextBox(float x, float y, float w, float h, float anchorX, float anchorY)   
    :IObject(x, y, w, h, anchorX, anchorY),font(Resources::GetInstance().GetFont("pirulen.ttf", 70)){
    }

    void TextBox::OnKeyDown(int keycode){
        if(name.length() >= maxLen){
            done = true;
            return;
        }
        if(keycode == ALLEGRO_KEY_ENTER && !name.empty()){
            done = true;
        }else if(keycode == ALLEGRO_KEY_BACKSPACE && !name.empty()){
            name.pop_back();
        }else if(keycode == ALLEGRO_KEY_SPACE){
            name += " ";
        }else if(keycode >= ALLEGRO_KEY_A && keycode <= ALLEGRO_KEY_Z){
            name += static_cast<char>(keycode-ALLEGRO_KEY_A + 'A');
        }else if(keycode >= ALLEGRO_KEY_0 && keycode <= ALLEGRO_KEY_9){
            name += std::to_string(keycode - ALLEGRO_KEY_0);
        }else if(keycode == ALLEGRO_KEY_FULLSTOP){
            name += '.';
        }
    }
    
    void TextBox::Draw() const{
        al_draw_filled_rounded_rectangle(Position.x, Position.y, Position.x+Size.x, Position.y+Size.y, 10, 10, al_map_rgb(255, 255, 255));
        //al_draw_rounded_rectangle(Position.x, Position.y, Position.x + Size.x, Position.y + Size.y, 10, 10, al_map_rgb(0, 0, 0), 2);
        al_draw_text(font.get(), al_map_rgb(0, 0, 0), Position.x+5, Position.y+5, ALLEGRO_ALIGN_LEFT, name.c_str());
        if(!done && showCursor){
            float w = al_get_text_width(font.get(), name.c_str());
            al_draw_rectangle(Position.x+5+w, Position.y+5, Position.x+5+w, Position.y+Size.y-5, al_map_rgb(0, 0, 0), 2);
        }
    }
    
    void TextBox::Update(float deltaTime){
        ticks += deltaTime;
        if(ticks >= 1){
            ticks = 0;
            showCursor = !showCursor;
        }
    }

}