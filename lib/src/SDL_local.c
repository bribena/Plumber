#include "SDL_local.h"
#include "debugmalloc.h"

bool InitializeSDL(void) {
    IMG_InitFlags imgFlags = IMG_INIT_JPG | IMG_INIT_PNG;
    MIX_InitFlags mixFlags = MIX_INIT_MP3;

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS);
    
    IMG_Init(imgFlags);
    TTF_Init();
    Mix_Init(mixFlags);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

    SDL_SetHint("SDL_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR", "0");

    if (!SDL_SetHint("SDL_HINT_RENDER_SCALE_QUALITY", "best")) 
        if (!SDL_SetHint("SDL_HINT_RENDER_SCALE_QUALITY", "linear"))
            SDL_SetHint("SDL_HINT_RENDER_SCALE_QUALITY", "nearest");

    return true;
}
void QuitSDL(void) {
    Mix_CloseAudio();
    Mix_Quit();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

