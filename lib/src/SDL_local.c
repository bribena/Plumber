#include "SDL_local.h"
#include "debugmalloc.h"

bool InitializeSDL(void) {
    IMG_InitFlags imgFlags = IMG_INIT_JPG | IMG_INIT_PNG;
    MIX_InitFlags mixFlags = MIX_INIT_MP3;

    if (!(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS) == 0 && IMG_Init(imgFlags) == (int)imgFlags && TTF_Init() == 0)) {
        printf("Inicializacios hiba: %s", SDL_GetError());
        QuitSDL();
        return false;
    }
    
    if (Mix_Init(mixFlags) != (int)mixFlags || Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) != 0) {
        SDL_LogWarn(SDL_LOG_CATEGORY_AUDIO, "SDL_Mixer hiba, nem lesz zene: %s", Mix_GetError());
        Mix_Quit();
    }

    if (!SDL_SetHint("SDL_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR", "0")) {
        SDL_LogWarn(SDL_LOG_CATEGORY_VIDEO, "A compositor leállhat.");
    }

    if (!SDL_SetHint("SDL_HINT_RENDER_SCALE_QUALITY", "best")) 
        if (!SDL_SetHint("SDL_HINT_RENDER_SCALE_QUALITY", "linear"))
            SDL_SetHint("SDL_HINT_RENDER_SCALE_QUALITY", "nearest");

    return true;
}

void StartMusic(Assets assets) {
    if (Mix_PlayMusic(assets.bgm, -1) != 0)
        SDL_LogWarn(SDL_LOG_CATEGORY_AUDIO, "Nem sikerült a bgm lejátszása: %s", Mix_GetError());
    else
        Mix_VolumeMusic(MIX_VOLUME);
}

void QuitSDL(void) {
    Mix_CloseAudio();
    Mix_Quit();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

