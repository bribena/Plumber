#include "game.h"
#include "debugmalloc.h"


Wheel Wheel_Init(void) {
    return (Wheel){Layer_Init(), 0};
}

void Wheel_Create(Wheel * wheel, Window window, SDL_Rect location, SDL_Texture * wheel_texture) {
    Layer_Create(&wheel->layer, window, &location);
    SDL_SetRenderTarget(window.renderer, wheel->layer.layer);
    SDL_RenderCopy(window.renderer, wheel_texture, NULL, NULL);
    SDL_SetRenderTarget(window.renderer, NULL);
}

void Wheel_Update(Wheel wheel, Window window, SDL_Texture * wheel_texture) {
    SDL_SetRenderTarget(window.renderer, wheel.layer.layer);
    SDL_SetRenderDrawColor(window.renderer, 0, 0, 0, 0);
    SDL_RenderClear(window.renderer);
    SDL_RenderCopyEx(window.renderer, wheel_texture, NULL, NULL, wheel.szog, NULL, SDL_FLIP_NONE);
    SDL_SetRenderTarget(window.renderer, NULL);
}

void Wheel_Destroy(Wheel * wheel) {
    Layer_Destroy(&wheel->layer);
}