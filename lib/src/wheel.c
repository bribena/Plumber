#include "game.h"
#include "debugmalloc.h"


Wheel Wheel_Init(void) {
    return (Wheel){Layer_Init(), 0};
}

bool Wheel_Create(Wheel * wheel, Window window, SDL_Rect location, SDL_Texture * wheel_texture) {
    wheel->szog = rand() % 360;
    bool ret =
        !Layer_Create(&wheel->layer, window, &location) ||
        SDL_SetRenderTarget(window.renderer, wheel->layer.layer) < 0 ||
        SDL_RenderCopyEx(window.renderer, wheel_texture, NULL, NULL, wheel->szog, NULL, SDL_FLIP_NONE) < 0;
    return !ret;
}

bool Wheel_Update(Wheel wheel, Window window, SDL_Texture * wheel_texture) {
    bool ret =
        SDL_SetRenderTarget(window.renderer, wheel.layer.layer) < 0 ||
        SDL_SetRenderDrawColor(window.renderer, 0, 0, 0, 0) < 0 ||
        SDL_RenderClear(window.renderer) < 0 ||
        SDL_RenderCopyEx(window.renderer, wheel_texture, NULL, NULL, wheel.szog, NULL, SDL_FLIP_NONE) < 0 ||
        SDL_SetRenderTarget(window.renderer, NULL) < 0;
    return !ret;
}

void Wheel_Destroy(Wheel * wheel) {
    Layer_Destroy(&wheel->layer);
}