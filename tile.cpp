#include "tile.h"

using namespace std;

Tile::Tile(const SDL_Rect& rect, const SDL_Color& colour, TTF_Font* const font, const SDL_Color& fontColour, const int number) 
    : Button(rect, colour, font, fontColour)
    {
        mNumber = number;
}

int Tile::getXPosition() {
    return mRect.x;
}

int Tile::getYPosition() {
    return mRect.y;
}                                                                                                                                                                                                                                                                      

bool Tile::moveTo(const int x, const int y) {
    // Di chuyen tile den muc tieu co toa do x, y tung doan mot la 1 hoac - 1 

    if (mRect.x != x) {
        const int delta = (x - mRect.x > 0) ? 1 : -1;
        mRect.x += delta;
        mFontRect.x += delta;
        return false;
    } 

    if (mRect.y != y) {
        const int delta = (y - mRect.y > 0) ? 1 : -1;
        mRect.y += delta;
        mFontRect.y += delta;
        return false;
    }

    // Neu da den noi thi la true, chua den thi false
    return true;
}

void Tile::setPositionTo(const int x, const int y) {
    mRect.x = x;
    mRect.y = y;
    centerText();
}

int Tile::getNumber() {
    return mNumber;
}