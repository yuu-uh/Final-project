#ifndef TEXTBOX_HPP
#define TEXTBOX_HPP

#include <allegro5/bitmap.h>
#include <allegro5/allegro_font.h>
#include <functional>
#include <memory>
#include <string>

#include "Engine/IControl.hpp"
#include "Image.hpp"

namespace Engine{
    class TextBox: public IControl, public IObject{
    public:
        std::shared_ptr<ALLEGRO_FONT> font;
        std::string name;
        bool done = false;
        bool showCursor = true;
        const int maxLen = 15;
        float ticks;

        TextBox(float x, float y, float w, float h, float anchorX, float anchorY);
        void OnKeyDown(int keycode) override;
        void Draw() const override;
        void Update(float deltaTime) override;
    };
};

#endif