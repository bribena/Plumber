#include "SDL_local.h"
#include "debugmalloc.h"


Layer Layer_Init(void) {
    return (Layer){NULL, (SDL_Rect){0,0,0,0}};
}

bool Layer_Create(Layer * layer, Window window, SDL_Rect * location) {
    SDL_Rect l;
    if (location == NULL)
        l = (SDL_Rect){0, 0, window.width, window.height};
    else
        l = *location;
    layer->layer = SDL_CreateTexture(window.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, l.w, l.h);
    SDL_SetTextureBlendMode(layer->layer, SDL_BLENDMODE_BLEND);
    layer->location = l;
    return true;
}

bool Layer_RenderFont(Layer * layer, Window window, TTF_Font * font, char * text, SDL_Color foreground) {
    SDL_Surface * surface = TTF_RenderUTF8_Blended(font, text, foreground);
    SDL_DestroyTexture(layer->layer);
    layer->layer = SDL_CreateTextureFromSurface(window.renderer, surface);
    SDL_FreeSurface(surface);
    return true;
}

void Layer_Destroy(Layer * layer) {
    SDL_DestroyTexture(layer->layer);
}