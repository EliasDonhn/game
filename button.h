#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "userInterface.h"

class Button : public UserInterface {
    public:
        Button(const SDL_Rect& rect, const SDL_Color& colour, TTF_Font* const font, const SDL_Color& fontColour);
        
        bool isMouseInside(const int x, const int y) const;
        void changeColourTo(const SDL_Colour& colour);
        void changeFontColourTo(const SDL_Colour& fontColour);

};