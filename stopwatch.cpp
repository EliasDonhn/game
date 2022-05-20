#include "stopwatch.h"

Stopwatch::Stopwatch(const SDL_Rect& rect, const SDL_Color& colour, TTF_Font* const font, const SDL_Color& fontColour) 
    :  UserInterface(rect, colour, font, fontColour),
    mStartTime(0), mElapsedTime("") {
    
}

void Stopwatch::start() {
    time(&mStartTime);
}

void Stopwatch::calculateTime(SDL_Renderer* const renderer, bool solved) {
    time_t difference = time(NULL) - mStartTime;
    struct tm* timeinfo = gmtime(&difference);
    strftime(mElapsedTime, sizeof(mElapsedTime), "%H:%M:%S", timeinfo);

    char attitude[] = "In danger ";
    // Load elapsed time into texture
    if(!solved)
        loadTexture(renderer, strcat(attitude, mElapsedTime));
    else
        loadTexture(renderer, "YOU'RE SAFE");
}
