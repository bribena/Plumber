#include "SDL_local.h"
#include "debugmalloc.h"


Fonts Fonts_Init(void) {
    return (Fonts){NULL, NULL, NULL, NULL};
}

bool Load_Font(TTF_Font ** font, char * path) {
    SDL_RWops * ops = SDL_RWFromFile(path, "r");
    *font = TTF_OpenFontRW(ops, 1, DEFAULT_FONT_SIZE);
    // SDL_Log(SDL_GetError());
    return true;
}

bool Fonts_Load(Fonts * fonts) {
    Load_Font(&fonts->normal, "assets/fonts/RedHatDisplay-Bold.ttf");
    Load_Font(&fonts->italic, "assets/fonts/RedHatDisplay-BoldItalic.ttf");
    Load_Font(&fonts->bold, "assets/fonts/RedHatDisplay-Black.ttf");
    Load_Font(&fonts->bold_italic, "assets/fonts/RedHatDisplay-BlackItalic.ttf");
    return true;
}

void Fonts_Destroy(Fonts * fonts) {
    TTF_CloseFont(fonts->bold);
    TTF_CloseFont(fonts->bold_italic);
    TTF_CloseFont(fonts->italic);
    TTF_CloseFont(fonts->normal);
}