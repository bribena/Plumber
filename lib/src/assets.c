#include "SDL_local.h"
#include "debugmalloc.h"

Assets Assets_Init(void) {
    return (Assets){NULL, NULL, NULL, NULL, NULL};
}


bool Load_Texture(SDL_Texture ** texture, SDL_Renderer * renderer, const char * path) {
    SDL_RWops * ops = SDL_RWFromFile(path, "r");
    SDL_Log(SDL_GetError());
    *texture = IMG_LoadTexture_RW(renderer, ops, 1);
    return true;
}
bool Load_Music(Mix_Music ** music, char * path) {
    SDL_RWops * ops = SDL_RWFromFile(path, "r");
    *music = Mix_LoadMUSType_RW(ops, MUS_MP3, 1);
    SDL_Log(SDL_GetError());
    return true;
}

bool Assets_Load(Assets * assets, SDL_Renderer * renderer) {
    Load_Texture(&assets->background, renderer, "assets/img/bg.jpg");
    Load_Texture(&assets->pipes_empty, renderer, "assets/img/sprites_empty.png");

    Load_Texture(&assets->pipes_full, renderer, "assets/img/sprites_empty.png");
    SDL_SetTextureColorMod(assets->pipes_full, 0, 160, 255);
    SDL_SetTextureAlphaMod(assets->pipes_full, 160);

    Load_Texture(&assets->wheel, renderer, "assets/img/wheel.png");
    Load_Music(&assets->bgm, "assets/audio/kahoot.mp3");
    return true;
}

void Assets_Destroy(Assets * assets) {
    SDL_DestroyTexture(assets->background);
    SDL_DestroyTexture(assets->pipes_empty);
    SDL_DestroyTexture(assets->pipes_full);
    SDL_DestroyTexture(assets->wheel);
    Mix_FreeMusic(assets->bgm);
}