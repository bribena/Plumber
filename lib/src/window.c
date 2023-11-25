/**
 * @file window.c
 * @brief Az SDL_local.h-ban deklarált Window struktúrához kapcsolódó függvények kódjait tartalmazza a fájl.
 */

#include "SDL_local.h"
#include "debugmalloc.h"


Window Window_Init(void) {
    return (Window){NULL, NULL, 0, 0};
}
bool Window_Create(Window * window, const char * title, int width, int height) {
    window->window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);
    if (window->window == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Sikertelen ablak létrehozás: %s", SDL_GetError());
        return false;
    }

    window->renderer = SDL_CreateRenderer(window->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE);
    if (window->renderer == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Sikertelen renderer létrehozás: %s", SDL_GetError());
        SDL_DestroyWindow(window->window);
        return false;
    }

    window->width = width;
    window->height = height;

    return true;
}
void Window_Destroy(Window * window) {
    SDL_DestroyRenderer(window->renderer);
    SDL_DestroyWindow(window->window);
}
