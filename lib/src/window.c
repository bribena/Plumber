#include "SDL_local.h"
#include "debugmalloc.h"


Window Window_Init(void) {
    return (Window){NULL, NULL, 0, 0};
}
bool Window_Create(Window * window, char * title, int width, int height) {
    window->window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);
    window->renderer = SDL_CreateRenderer(window->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE);
    window->width = width;
    window->height = height;

    SDL_SetRenderDrawColor(window->renderer, 0, 0, 0, 255);
    SDL_SetRenderDrawBlendMode(window->renderer, SDL_BLENDMODE_BLEND);

    return true;
}
void Window_Destroy(Window * window) {
    SDL_DestroyRenderer(window->renderer);
    SDL_DestroyWindow(window->window);
}
