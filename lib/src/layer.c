/**
 * @file layer.c
 * @brief Az SDL_local.h-ban deklarált Layer struktúrához kapcsolódó függvények kódjait tartalmazza a fájl.
*/

#include "SDL_local.h"
#include "debugmalloc.h"


Layer Layer_Init(void) {
    return (Layer){NULL, (SDL_Rect){0,0,0,0}};
}

bool Layer_Create(Layer * layer, Window window, const SDL_Rect * location) {
    SDL_Rect l;
    if (location == NULL)
        l = (SDL_Rect){0, 0, window.width, window.height};
    else
        l = *location;
    layer->layer = SDL_CreateTexture(window.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, l.w, l.h);
    if (layer->layer == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_RENDER, "Nem sikerült a textúra létrehozása: %s", SDL_GetError());
        return false;
    }
    if (SDL_SetTextureBlendMode(layer->layer, SDL_BLENDMODE_BLEND) < 0)
        SDL_LogWarn(SDL_LOG_CATEGORY_RENDER, "Nem sikerült egy textúra BlendMode beállítás: %s", SDL_GetError());
    layer->location = l;
    return true;
}

bool Layer_RenderFont(Layer * layer, Window window, TTF_Font * font, const char * text, SDL_Color foreground) {
    SDL_Surface * surface = TTF_RenderUTF8_Blended(font, text, foreground);
    if (surface == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Nem sikerült a felirat létrehozása: %s", SDL_GetError());
        return false;
    }
    SDL_DestroyTexture(layer->layer);
    layer->layer = SDL_CreateTextureFromSurface(window.renderer, surface);
    if (layer->layer == NULL) 
        SDL_LogError(SDL_LOG_CATEGORY_RENDER, "Nem sikerült a Surface konverzió: %s", SDL_GetError());
    SDL_FreeSurface(surface);
    return layer->layer != NULL;
}

void Layer_Destroy(Layer * layer) {
    if (layer->layer == NULL) return;
    SDL_DestroyTexture(layer->layer);
    layer->layer = NULL;
    layer->location = (SDL_Rect){0,0,0,0};
}