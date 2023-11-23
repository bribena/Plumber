#include "SDL_local.h"
#include "debugmalloc.h"

Assets Assets_Init(void) {
    return (Assets){NULL, NULL, NULL, NULL, NULL};
}


bool Load_Texture(SDL_Texture ** texture, SDL_Renderer * renderer, const char * path) {
    SDL_RWops * ops = SDL_RWFromFile(path, "r");
    if (ops == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Sikertelen fájl megnyitás: %s", SDL_GetError());
        return false;
    }

    *texture = IMG_LoadTexture_RW(renderer, ops, 1);
    if (*texture == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Sikertelen textúra betöltés: %s", SDL_GetError());
        return false;
    }
    return true;
}
bool Load_Music(Mix_Music ** music, char * path) {
    SDL_RWops * ops = SDL_RWFromFile(path, "r");
    if (ops == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Sikertelen fájl megnyitás: %s", SDL_GetError());
        return false;
    }
    
    *music = Mix_LoadMUSType_RW(ops, MUS_MP3, 1);
    if (*music == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Sikertelen zene betöltés: %s", Mix_GetError());
        return false;
    }
    return true;
}

bool Assets_Load(Assets * assets, Window window) {
    if (!Load_Texture(&assets->background, window.renderer, "assets/img/bg.jpg")) {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Háttér betöltése sikertelen, nem lesz háttér.");
        assets->background = SDL_CreateTexture(window.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC, window.width, window.height);
        if (assets->background == NULL) {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Nem sikerült a háttér pótlása: %s", SDL_GetError());
            return false;
        }
    }

    if (!Load_Texture(&assets->pipes_empty, window.renderer, "assets/img/sprites_empty.png")) {
        SDL_DestroyTexture(assets->background);
        return false;
    } 
    if (!Load_Texture(&assets->pipes_full, window.renderer, "assets/img/sprites_empty.png")) {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Nem sikerült egy betöltés, hiányozni fog animáció.");
        assets->pipes_full = assets->pipes_empty;
    }

    if (assets->pipes_full != assets->pipes_empty && (SDL_SetTextureColorMod(assets->pipes_full, 0, 160, 255) < 0 || SDL_SetTextureAlphaMod(assets->pipes_full, 160) < 0)) {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Sikertelen szinezés, hibás lehet az animáció: %s", SDL_GetError());
        SDL_DestroyTexture(assets->pipes_full);
        assets->pipes_full = assets->pipes_empty;
    }

    if (!Load_Texture(&assets->wheel, window.renderer, "assets/img/wheel.png")) {
        SDL_DestroyTexture(assets->background);
        if (assets->pipes_full != assets->pipes_empty)
            SDL_DestroyTexture(assets->pipes_full);
        SDL_DestroyTexture(assets->pipes_empty);
        return false;
    } 

    if (!Load_Music(&assets->bgm, "assets/audio/kahoot.mp3")) {
        SDL_LogWarn(SDL_LOG_CATEGORY_AUDIO, "Nem sikerült a zene betöltése, nem lesz zene.");
        assets->bgm = NULL;
    }

    return true;
}

void Assets_Destroy(Assets * assets) {
    SDL_DestroyTexture(assets->background);
    SDL_DestroyTexture(assets->pipes_empty);
    SDL_DestroyTexture(assets->pipes_full);
    SDL_DestroyTexture(assets->wheel);
    Mix_FreeMusic(assets->bgm);
}