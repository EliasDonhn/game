#include "userInterface.h"

using namespace std;

UserInterface::UserInterface(const SDL_Rect& rect, const SDL_Color& colour, TTF_Font* const font, const SDL_Color& fontColour) 
{
    mRect = rect;
    mColour = colour;
    mFontRect = {0, 0, 0, 0};
    mFontColour = fontColour;
    mFont = font;
    mTexture = nullptr;
}

void UserInterface::loadTexture(SDL_Renderer* const renderer, const char* text) {
    free(); 

    SDL_Surface* textSurface = TTF_RenderText_Solid(mFont, text, mFontColour);
    if (textSurface == nullptr)
        cout << "Unable to render text surface! Error: " << TTF_GetError() << endl; // Khi TTF_RenderText_Solid loi thi tra ve nullptr
    else {
        mTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        if (mTexture == nullptr)
            cout << "Unable to create texture form rendered text! Error: " << SDL_GetError() << endl;
        else 
        {
            mFontRect.w = textSurface->w;
            mFontRect.h = textSurface->h;
        }
        SDL_FreeSurface(textSurface);
    }
    centerText();
}

void UserInterface::centerText() {
    // Can chinh cho van ban nam o vi tri center
    mFontRect.y = mRect.y + 0.5 * (mRect.h - mFontRect.h);
	mFontRect.x = mRect.x + 0.5 * (mRect.w - mFontRect.w);
}

void UserInterface::render(SDL_Renderer* const renderer) const {
    // Render ra giao dien nguoi dung
    SDL_SetRenderDrawColor(renderer, mColour.r, mColour.g, mColour.b, mColour.a);
    SDL_RenderFillRect(renderer, &mRect);

    if (mTexture != nullptr)
        SDL_RenderCopy(renderer, mTexture, nullptr, &mFontRect);
    else
        cout << "Warning: no texture to render!" << endl;
}

void UserInterface::free() {
    if (mTexture != nullptr) {
        SDL_DestroyTexture(mTexture);
        mTexture = nullptr;
    }
}
