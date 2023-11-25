/**
 * @file fonts.c
 * @brief Az SDL_local.h-ban deklarált Fonts struktúrához kapcsolódó függvények kódjait tartalmazza a fájl.
 */

#include "SDL_local.h"
#include "debugmalloc.h"


Fonts Fonts_Init(void) {
    return (Fonts){NULL, NULL, NULL, NULL};
}

static bool Load_Font(TTF_Font ** font, const char * path) {
    SDL_RWops * ops = SDL_RWFromFile(path, "r");
    if (ops == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Sikertelen fájl megnyitás: %s", SDL_GetError());
        return false;
    }

    *font = TTF_OpenFontRW(ops, 1, DEFAULT_FONT_SIZE);
    if (*font == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Sikertelen font betöltés: %s", TTF_GetError());
        return false;
    }

    return true;
}

bool Fonts_Load(Fonts * fonts) {
    // Load_Font(&fonts->normal, "assets/fonts/RedHatDisplay-Bold.ttf");
    // Load_Font(&fonts->italic, "assets/fonts/RedHatDisplay-BoldItalic.ttf");
    // Load_Font(&fonts->bold_italic, "assets/fonts/RedHatDisplay-BlackItalic.ttf");
    
    if (!Load_Font(&fonts->bold, "assets/fonts/RedHatDisplay-Black.ttf")) return false;
    return true;
}

void Fonts_Destroy(Fonts * fonts) {
    TTF_CloseFont(fonts->bold);
    // TTF_CloseFont(fonts->bold_italic);
    // TTF_CloseFont(fonts->italic);
    // TTF_CloseFont(fonts->normal);
}