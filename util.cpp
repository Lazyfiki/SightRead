#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <chrono>
#include <random>

double getCurrentTime() {
    return std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}

int getRandomNumber(int min, int max) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(min, max);

    return dis(gen);
}

void renderText(SDL_Renderer* renderer, const char* text, SDL_Color color, int x, int y) {
    const char* fontPath = "./assets/fonts/NotoMusic-Regular.ttf";
    int fontSize = 134;

    TTF_Font* font = TTF_OpenFont(fontPath, fontSize);
    if (!font) {
        std::cerr << "TTF_OpenFont Error: " << TTF_GetError() << std::endl;
        return;
    }

    SDL_Surface* surface = TTF_RenderUTF8_Blended(font, text, color);
    if (!surface) {
        std::cerr << "TTF_RenderUTF8_Blended Error: " << TTF_GetError() << std::endl;
        TTF_CloseFont(font);
        return;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        std::cerr << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(surface);
        TTF_CloseFont(font);
        return;
    }

    SDL_Rect destRect = { x, y, surface->w, surface->h };
    SDL_RenderCopy(renderer, texture, nullptr, &destRect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
    TTF_CloseFont(font);
}
